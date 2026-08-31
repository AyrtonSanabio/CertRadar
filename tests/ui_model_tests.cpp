#include "certradar/ui_model.hpp"

#include <doctest/doctest.h>

TEST_CASE("candidate label explains classification and keeps the local path visible") {
    const certradar::SearchCandidate candidate{
        "C:/Users/Teste/Downloads/certificado.pfx", 100, certradar::CandidateState::recognized};

    const auto label = certradar::format_candidate_label(candidate);

    CHECK(label.find(L"contêiner reconhecido") != std::wstring::npos);
    CHECK(label.find(L"certificado.pfx") != std::wstring::npos);
}

TEST_CASE("search support summary reports counts without exposing local paths") {
    certradar::SearchResult result;
    result.status = certradar::ScanStatus::completed;
    result.directories_visited = 42;
    result.access_denied_count = 3;
    result.errors = 1;
    result.candidates = {
        {"C:/Users/Maria/Downloads/12345678901.pfx", 100,
         certradar::CandidateState::recognized},
        {"D:/Segredos/cliente-senha.p12", 200, certradar::CandidateState::invalid},
        {"E:/arquivo-grande.pfx", 40ULL * 1024ULL * 1024ULL,
         certradar::CandidateState::too_large},
    };

    const auto summary = certradar::build_search_support_summary(result);

    CHECK(summary.find(L"Busca concluída") != std::wstring::npos);
    CHECK(summary.find(L"Candidatos encontrados: 3") != std::wstring::npos);
    CHECK(summary.find(L"Reconhecidos: 1") != std::wstring::npos);
    CHECK(summary.find(L"Inválidos: 1") != std::wstring::npos);
    CHECK(summary.find(L"Acima do limite: 1") != std::wstring::npos);
    CHECK(summary.find(L"Pastas verificadas: 42") != std::wstring::npos);
    CHECK(summary.find(L"Pastas sem acesso: 3") != std::wstring::npos);
    CHECK(summary.find(L"Erros isolados: 1") != std::wstring::npos);
    CHECK(summary.find(L"C:/Users/Maria") == std::wstring::npos);
    CHECK(summary.find(L"12345678901.pfx") == std::wstring::npos);
    CHECK(summary.find(L"cliente-senha.p12") == std::wstring::npos);

    certradar::WindowsPlatform platform{10, 0, 19045};
    platform.architecture = "x64";
    const auto contextual_summary = certradar::build_search_support_summary(result, platform);
    CHECK(contextual_summary.find(L"Windows 10") != std::wstring::npos);
    CHECK(contextual_summary.find(L"build 19045") != std::wstring::npos);
    CHECK(contextual_summary.find(L"x64") != std::wstring::npos);
    CHECK(contextual_summary.find(L"12345678901.pfx") == std::wstring::npos);
    CHECK(contextual_summary.find(L"cliente-senha.p12") == std::wstring::npos);
}

TEST_CASE("candidate reveal plan only accepts a selected absolute search result") {
    certradar::SearchResult result;
    result.candidates = {
        {"C:/Users/Teste/Downloads/certificado.pfx", 100,
         certradar::CandidateState::recognized},
        {"caminho/relativo.p12", 200, certradar::CandidateState::invalid},
        {"C:/Users/Teste/../Outro/certificado.pfx", 300,
         certradar::CandidateState::recognized},
    };

    const auto ready = certradar::build_candidate_reveal_plan(result, 0);
    CHECK(ready.status == certradar::CandidateRevealStatus::ready);
    CHECK(ready.path == result.candidates[0].path);

    const auto relative = certradar::build_candidate_reveal_plan(result, 1);
    CHECK(relative.status == certradar::CandidateRevealStatus::unsafe_path);
    CHECK(relative.path.empty());

    const auto traversal = certradar::build_candidate_reveal_plan(result, 2);
    CHECK(traversal.status == certradar::CandidateRevealStatus::unsafe_path);
    CHECK(traversal.path.empty());

    const auto missing = certradar::build_candidate_reveal_plan(result, 99);
    CHECK(missing.status == certradar::CandidateRevealStatus::no_selection);
    CHECK(missing.path.empty());
}

TEST_CASE("platform summary translates Windows versions support and privilege for support") {
    certradar::WindowsPlatform windows_10{10, 0, 19045};
    windows_10.architecture = "x64";
    const auto ten = certradar::format_platform_summary(windows_10);
    CHECK(ten.find(L"Windows 10") != std::wstring::npos);
    CHECK(ten.find(L"build 19045") != std::wstring::npos);
    CHECK(ten.find(L"x64") != std::wstring::npos);
    CHECK(ten.find(L"suporte completo") != std::wstring::npos);
    CHECK(ten.find(L"usuário comum") != std::wstring::npos);

    certradar::WindowsPlatform windows_11{10, 0, 26100};
    windows_11.architecture = "arm64";
    windows_11.elevated = true;
    const auto eleven = certradar::format_platform_summary(windows_11);
    CHECK(eleven.find(L"Windows 11") != std::wstring::npos);
    CHECK(eleven.find(L"arm64") != std::wstring::npos);
    CHECK(eleven.find(L"administrador") != std::wstring::npos);

    certradar::WindowsPlatform windows_7{6, 1, 7601};
    windows_7.service_pack_major = 1;
    CHECK(certradar::format_platform_summary(windows_7).find(L"Windows 7 SP1") !=
          std::wstring::npos);

    certradar::WindowsPlatform windows_xp{5, 1, 2600};
    windows_xp.service_pack_major = 3;
    CHECK(certradar::format_platform_summary(windows_xp).find(L"Windows XP SP3") !=
          std::wstring::npos);
    CHECK(certradar::format_platform_summary(windows_xp).find(L"modo legado") !=
          std::wstring::npos);
}

TEST_CASE("installed certificate summary exposes support facts without personal identity") {
    certradar::CertificateRecord certificate;
    certificate.subject = "Maria da Silva 12345678900";
    certificate.issuer = "Autoridade Confidencial";
    certificate.serial_number = "00112233445566778899";
    certificate.thumbprint = "0123456789ABCDEF";
    certificate.valid_until = "2027-08-30T12:00:00Z";
    certificate.validity = certradar::CertificateValidity::valid;
    certificate.has_private_key_association = true;
    certificate.provider = "Provider com nome sensivel";
    certificate.provider_kind = certradar::ProviderKind::ksp;

    const auto summary = certradar::format_certificate_summary(certificate, 2);

    CHECK(summary.find(L"Certificado 2") != std::wstring::npos);
    CHECK(summary.find(L"89ABCDEF") != std::wstring::npos);
    CHECK(summary.find(L"válido") != std::wstring::npos);
    CHECK(summary.find(L"com chave privada associada") != std::wstring::npos);
    CHECK(summary.find(L"KSP") != std::wstring::npos);
    CHECK(summary.find(L"2027-08-30") != std::wstring::npos);
    CHECK(summary.find(L"Maria") == std::wstring::npos);
    CHECK(summary.find(L"12345678900") == std::wstring::npos);
    CHECK(summary.find(L"Autoridade Confidencial") == std::wstring::npos);
    CHECK(summary.find(L"00112233445566778899") == std::wstring::npos);
    CHECK(summary.find(L"Provider com nome sensivel") == std::wstring::npos);
    CHECK(summary.find(L"0123456789ABCDEF") == std::wstring::npos);
}

TEST_CASE("installed certificate store support summary contains counts without identity") {
    certradar::CertificateRecord valid;
    valid.subject = "Maria da Silva 12345678900";
    valid.issuer = "Autoridade Confidencial";
    valid.serial_number = "SERIAL-SENSIVEL";
    valid.thumbprint = "0123456789ABCDEF";
    valid.provider = "Provider completo sensivel";
    valid.validity = certradar::CertificateValidity::valid;
    valid.has_private_key_association = true;
    valid.provider_kind = certradar::ProviderKind::ksp;

    certradar::CertificateRecord expired;
    expired.subject = "Joao Cliente 98765432100";
    expired.thumbprint = "FEDCBA9876543210";
    expired.validity = certradar::CertificateValidity::expired;
    expired.provider_kind = certradar::ProviderKind::csp;

    certradar::CertificateStoreResult result;
    result.opened = true;
    result.certificates = {valid, expired};

    certradar::WindowsPlatform platform{10, 0, 19045};
    platform.architecture = "x64";
    const auto summary =
        certradar::build_certificate_store_support_summary(result, platform);

    CHECK(summary.find(L"Certificados instalados: 2") != std::wstring::npos);
    CHECK(summary.find(L"Válidos: 1") != std::wstring::npos);
    CHECK(summary.find(L"Expirados: 1") != std::wstring::npos);
    CHECK(summary.find(L"Com chave associada: 1") != std::wstring::npos);
    CHECK(summary.find(L"Sem chave associada: 1") != std::wstring::npos);
    CHECK(summary.find(L"KSP: 1") != std::wstring::npos);
    CHECK(summary.find(L"CSP: 1") != std::wstring::npos);
    CHECK(summary.find(L"Windows 10") != std::wstring::npos);
    CHECK(summary.find(L"Maria") == std::wstring::npos);
    CHECK(summary.find(L"12345678900") == std::wstring::npos);
    CHECK(summary.find(L"Autoridade Confidencial") == std::wstring::npos);
    CHECK(summary.find(L"SERIAL-SENSIVEL") == std::wstring::npos);
    CHECK(summary.find(L"0123456789ABCDEF") == std::wstring::npos);
    CHECK(summary.find(L"Provider completo sensivel") == std::wstring::npos);
    CHECK(summary.find(L"Joao") == std::wstring::npos);
    CHECK(summary.find(L"98765432100") == std::wstring::npos);
    CHECK(summary.find(L"FEDCBA9876543210") == std::wstring::npos);
}

TEST_CASE("installed certificates display problems before healthy credentials") {
    certradar::CertificateStoreResult result;
    result.opened = true;
    result.certificates.resize(6);
    result.certificates[0].validity = certradar::CertificateValidity::valid;
    result.certificates[0].has_private_key_association = true;
    result.certificates[1].validity = certradar::CertificateValidity::valid;
    result.certificates[1].has_private_key_association = false;
    result.certificates[2].validity = certradar::CertificateValidity::expiring_soon;
    result.certificates[3].validity = certradar::CertificateValidity::not_yet_valid;
    result.certificates[4].validity = certradar::CertificateValidity::expired;
    result.certificates[5].validity = certradar::CertificateValidity::expired;

    const auto order = certradar::build_certificate_display_order(result);

    REQUIRE(order.size() == 6);
    CHECK(order[0] == 4);
    CHECK(order[1] == 5);
    CHECK(order[2] == 3);
    CHECK(order[3] == 2);
    CHECK(order[4] == 1);
    CHECK(order[5] == 0);
}

TEST_CASE("certificate store summary identifies the selected Windows scope") {
    certradar::CertificateStoreResult machine;
    machine.scope = certradar::StoreScope::local_machine;
    machine.opened = true;

    const auto summary =
        certradar::build_certificate_store_support_summary(machine);

    CHECK(summary.find(L"Store Pessoal da máquina: acessível") != std::wstring::npos);
    CHECK(summary.find(L"Store Pessoal do usuário") == std::wstring::npos);
}

TEST_CASE("A3 local summary reports counts without reader names") {
    certradar::A3LocalSnapshot snapshot;
    snapshot.service.state = certradar::ServiceState::running;
    snapshot.readers_queried = true;
    snapshot.readers.success = true;
    snapshot.readers.readers = {
        {L"Leitor confidencial 12345678900", true, false, 0},
        {L"Token pessoal Maria", false, true, 0},
    };

    certradar::WindowsPlatform platform{10, 0, 19045};
    platform.architecture = "x64";
    const auto summary = certradar::build_a3_support_summary(snapshot, platform);

    CHECK(summary.find(L"Serviço de cartão inteligente: em execução") != std::wstring::npos);
    CHECK(summary.find(L"Leitores detectados: 2") != std::wstring::npos);
    CHECK(summary.find(L"Cartões/tokens presentes: 1") != std::wstring::npos);
    CHECK(summary.find(L"Leitores indisponíveis: 1") != std::wstring::npos);
    CHECK(summary.find(L"Windows 10") != std::wstring::npos);
    CHECK(summary.find(L"Leitor confidencial") == std::wstring::npos);
    CHECK(summary.find(L"12345678900") == std::wstring::npos);
    CHECK(summary.find(L"Token pessoal Maria") == std::wstring::npos);
}

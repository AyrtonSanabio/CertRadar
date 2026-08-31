#include "certradar/ui_model.hpp"

#include <algorithm>
#include <array>

namespace certradar {
namespace {

std::wstring scan_status_summary(const ScanStatus status) {
    switch (status) {
        case ScanStatus::not_started: return L"Busca não iniciada";
        case ScanStatus::running: return L"Busca em andamento";
        case ScanStatus::paused: return L"Busca pausada";
        case ScanStatus::completed: return L"Busca concluída";
        case ScanStatus::partial: return L"Busca concluída parcialmente";
        case ScanStatus::cancelled: return L"Busca cancelada com resultados parciais";
        case ScanStatus::failed: return L"Busca encerrada com falha";
    }
    return L"Estado da busca desconhecido";
}

std::wstring support_mode_summary(const SupportMode mode) {
    switch (mode) {
        case SupportMode::full: return L"suporte completo";
        case SupportMode::compatible: return L"suporte compatível";
        case SupportMode::legacy: return L"modo legado";
        case SupportMode::unsupported: return L"não suportado";
    }
    return L"suporte desconhecido";
}

std::wstring windows_name(const WindowsPlatform& platform) {
    std::wstring name;
    if (platform.major == 10 && platform.minor == 0) {
        name = platform.build >= 22000 ? L"Windows 11" : L"Windows 10";
    } else if (platform.major == 6 && platform.minor == 3) {
        name = L"Windows 8.1";
    } else if (platform.major == 6 && platform.minor == 1) {
        name = L"Windows 7";
    } else if (platform.major == 5 && platform.minor == 1) {
        name = L"Windows XP";
    } else {
        name = L"Windows " + std::to_wstring(platform.major) + L"." +
               std::to_wstring(platform.minor);
    }
    if (platform.service_pack_major > 0) {
        name += L" SP" + std::to_wstring(platform.service_pack_major);
    }
    return name;
}

std::wstring widen_ascii(const std::string& value) {
    return {value.begin(), value.end()};
}

std::wstring certificate_validity_summary(const CertificateValidity validity) {
    switch (validity) {
        case CertificateValidity::not_yet_valid: return L"ainda não válido";
        case CertificateValidity::valid: return L"válido";
        case CertificateValidity::expiring_soon: return L"vence em até 30 dias";
        case CertificateValidity::expired: return L"expirado";
    }
    return L"validade desconhecida";
}

std::wstring provider_kind_summary(const ProviderKind kind) {
    switch (kind) {
        case ProviderKind::csp: return L"CSP";
        case ProviderKind::ksp: return L"KSP";
        case ProviderKind::unknown: return L"provider desconhecido";
    }
    return L"provider desconhecido";
}

int certificate_support_priority(const CertificateRecord& certificate) noexcept {
    switch (certificate.validity) {
        case CertificateValidity::expired: return 0;
        case CertificateValidity::not_yet_valid: return 1;
        case CertificateValidity::expiring_soon: return 2;
        case CertificateValidity::valid:
            return certificate.has_private_key_association ? 4 : 3;
    }
    return 5;
}

std::wstring smart_card_service_summary(const ServiceState state) {
    switch (state) {
        case ServiceState::running: return L"em execução";
        case ServiceState::stopped: return L"parado";
        case ServiceState::pending: return L"alterando estado";
        case ServiceState::missing: return L"não encontrado";
        case ServiceState::inaccessible: return L"inacessível";
        case ServiceState::unknown: return L"estado desconhecido";
    }
    return L"estado desconhecido";
}

}  // namespace

std::wstring candidate_state_label(const CandidateState state) {
    switch (state) {
        case CandidateState::extension_match: return L"candidato";
        case CandidateState::recognized: return L"contêiner reconhecido";
        case CandidateState::invalid: return L"arquivo inválido";
        case CandidateState::too_large: return L"acima do limite";
        case CandidateState::inaccessible: return L"inacessível";
    }
    return L"estado desconhecido";
}

std::wstring format_candidate_label(const SearchCandidate& candidate) {
    return L"[" + candidate_state_label(candidate.state) + L"] " + candidate.path.wstring();
}

std::wstring build_search_support_summary(const SearchResult& result) {
    std::array<std::uint64_t, 5> states{};
    for (const auto& candidate : result.candidates) {
        const auto index = static_cast<std::size_t>(candidate.state);
        if (index < states.size()) ++states[index];
    }

    std::wstring summary = L"Resumo CertRadar — busca de certificado A1\r\n";
    summary += scan_status_summary(result.status) + L"\r\n";
    summary += L"Candidatos encontrados: " + std::to_wstring(result.candidates.size()) + L"\r\n";
    summary += L"Reconhecidos: " +
               std::to_wstring(states[static_cast<std::size_t>(CandidateState::recognized)]) +
               L"\r\n";
    summary += L"Inválidos: " +
               std::to_wstring(states[static_cast<std::size_t>(CandidateState::invalid)]) +
               L"\r\n";
    summary += L"Acima do limite: " +
               std::to_wstring(states[static_cast<std::size_t>(CandidateState::too_large)]) +
               L"\r\n";
    summary += L"Inacessíveis: " +
               std::to_wstring(states[static_cast<std::size_t>(CandidateState::inaccessible)]) +
               L"\r\n";
    summary += L"Pastas verificadas: " + std::to_wstring(result.directories_visited) + L"\r\n";
    summary += L"Pastas sem acesso: " + std::to_wstring(result.access_denied_count) + L"\r\n";
    summary += L"Erros isolados: " + std::to_wstring(result.errors) + L"\r\n";
    summary += L"Privacidade: nomes e caminhos locais não foram incluídos; nenhum arquivo foi alterado.\r\n";
    return summary;
}

std::wstring build_search_support_summary(
    const SearchResult& result,
    const WindowsPlatform& platform) {
    auto summary = build_search_support_summary(result);
    const auto first_line = summary.find(L"\r\n");
    const auto insertion = first_line == std::wstring::npos ? summary.size() : first_line + 2;
    summary.insert(insertion, format_platform_summary(platform) + L"\r\n");
    return summary;
}

CandidateRevealPlan build_candidate_reveal_plan(
    const SearchResult& result,
    const std::size_t selection_index) {
    if (selection_index >= result.candidates.size()) return {};

    const auto& path = result.candidates[selection_index].path;
    if (path.empty() || !path.is_absolute()) {
        return {CandidateRevealStatus::unsafe_path, {}};
    }
    for (const auto& component : path) {
        if (component == L"..") return {CandidateRevealStatus::unsafe_path, {}};
    }
    return {CandidateRevealStatus::ready, path.lexically_normal()};
}

std::wstring format_platform_summary(const WindowsPlatform& platform) {
    std::wstring summary = L"Ambiente: " + windows_name(platform);
    summary += L" (build " + std::to_wstring(platform.build) + L")";
    summary += L" " + widen_ascii(platform.architecture);
    summary += L" — " + support_mode_summary(classify_support_mode(platform));
    summary += platform.elevated ? L" — administrador" : L" — usuário comum";
    return summary;
}

std::wstring format_certificate_summary(
    const CertificateRecord& certificate,
    const std::size_t display_index) {
    const auto suffix = certificate.thumbprint.size() > 8
        ? certificate.thumbprint.substr(certificate.thumbprint.size() - 8)
        : certificate.thumbprint;
    std::wstring summary = L"Certificado " + std::to_wstring(display_index);
    summary += suffix.empty() ? L" — identificador indisponível" :
        L" — final " + widen_ascii(suffix);
    summary += L" — " + certificate_validity_summary(certificate.validity);
    summary += certificate.has_private_key_association
        ? L" — com chave privada associada"
        : L" — sem chave privada associada";
    summary += L" — " + provider_kind_summary(certificate.provider_kind);
    if (!certificate.valid_until.empty()) {
        summary += L" — vence " + widen_ascii(certificate.valid_until);
    }
    return summary;
}

std::wstring build_certificate_store_support_summary(
    const CertificateStoreResult& result) {
    std::array<std::uint64_t, 4> validity{};
    std::array<std::uint64_t, 3> providers{};
    std::uint64_t with_key = 0;
    for (const auto& certificate : result.certificates) {
        const auto validity_index = static_cast<std::size_t>(certificate.validity);
        if (validity_index < validity.size()) ++validity[validity_index];
        const auto provider_index = static_cast<std::size_t>(certificate.provider_kind);
        if (provider_index < providers.size()) ++providers[provider_index];
        if (certificate.has_private_key_association) ++with_key;
    }

    std::wstring summary = L"Resumo CertRadar — certificados instalados\r\n";
    const std::wstring store_name = result.scope == StoreScope::current_user
        ? L"Store Pessoal do usuário"
        : L"Store Pessoal da máquina";
    summary += store_name + (result.opened ? L": acessível\r\n" : L": indisponível\r\n");
    summary += L"Certificados instalados: " +
               std::to_wstring(result.certificates.size()) + L"\r\n";
    summary += L"Ainda não válidos: " + std::to_wstring(
        validity[static_cast<std::size_t>(CertificateValidity::not_yet_valid)]) + L"\r\n";
    summary += L"Válidos: " + std::to_wstring(
        validity[static_cast<std::size_t>(CertificateValidity::valid)]) + L"\r\n";
    summary += L"Vencem em até 30 dias: " + std::to_wstring(
        validity[static_cast<std::size_t>(CertificateValidity::expiring_soon)]) + L"\r\n";
    summary += L"Expirados: " + std::to_wstring(
        validity[static_cast<std::size_t>(CertificateValidity::expired)]) + L"\r\n";
    summary += L"Com chave associada: " + std::to_wstring(with_key) + L"\r\n";
    summary += L"Sem chave associada: " +
               std::to_wstring(result.certificates.size() - with_key) + L"\r\n";
    summary += L"CSP: " + std::to_wstring(
        providers[static_cast<std::size_t>(ProviderKind::csp)]) + L"\r\n";
    summary += L"KSP: " + std::to_wstring(
        providers[static_cast<std::size_t>(ProviderKind::ksp)]) + L"\r\n";
    summary += L"Provider desconhecido: " + std::to_wstring(
        providers[static_cast<std::size_t>(ProviderKind::unknown)]) + L"\r\n";
    summary += L"Privacidade: identidade, emissor, série, impressão digital e provider completo não foram incluídos; nenhuma chave foi acessada.\r\n";
    return summary;
}

std::wstring build_certificate_store_support_summary(
    const CertificateStoreResult& result,
    const WindowsPlatform& platform) {
    auto summary = build_certificate_store_support_summary(result);
    const auto first_line = summary.find(L"\r\n");
    const auto insertion = first_line == std::wstring::npos ? summary.size() : first_line + 2;
    summary.insert(insertion, format_platform_summary(platform) + L"\r\n");
    return summary;
}

std::vector<std::size_t> build_certificate_display_order(
    const CertificateStoreResult& result) {
    std::vector<std::size_t> order;
    order.reserve(result.certificates.size());
    for (std::size_t index = 0; index < result.certificates.size(); ++index) {
        order.push_back(index);
    }
    std::stable_sort(order.begin(), order.end(), [&result](const auto left, const auto right) {
        return certificate_support_priority(result.certificates[left]) <
               certificate_support_priority(result.certificates[right]);
    });
    return order;
}

std::wstring build_a3_support_summary(const A3LocalSnapshot& snapshot) {
    std::uint64_t cards_present = 0;
    std::uint64_t unavailable = 0;
    for (const auto& reader : snapshot.readers.readers) {
        if (reader.card_present) ++cards_present;
        if (reader.unavailable) ++unavailable;
    }

    std::wstring summary = L"Resumo CertRadar — diagnóstico A3 local\r\n";
    summary += L"Serviço de cartão inteligente: " +
               smart_card_service_summary(snapshot.service.state) + L"\r\n";
    if (!snapshot.readers_queried) {
        summary += L"Leitores: não consultados porque o serviço não está em execução\r\n";
    } else if (!snapshot.readers.success) {
        summary += L"Leitores: consulta indisponível; código " +
                   std::to_wstring(snapshot.readers.error_code) + L"\r\n";
    } else {
        summary += L"Leitores detectados: " +
                   std::to_wstring(snapshot.readers.readers.size()) + L"\r\n";
        summary += L"Cartões/tokens presentes: " + std::to_wstring(cards_present) + L"\r\n";
        summary += L"Leitores indisponíveis: " + std::to_wstring(unavailable) + L"\r\n";
    }
    summary += L"Privacidade: nomes de leitores não foram incluídos; nenhuma sessão, chave ou tentativa de PIN foi usada.\r\n";
    return summary;
}

std::wstring build_a3_support_summary(
    const A3LocalSnapshot& snapshot,
    const WindowsPlatform& platform) {
    auto summary = build_a3_support_summary(snapshot);
    const auto first_line = summary.find(L"\r\n");
    const auto insertion = first_line == std::wstring::npos ? summary.size() : first_line + 2;
    summary.insert(insertion, format_platform_summary(platform) + L"\r\n");
    return summary;
}

}  // namespace certradar

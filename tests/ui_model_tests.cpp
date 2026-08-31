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
}

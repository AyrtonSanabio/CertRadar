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

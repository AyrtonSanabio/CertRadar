#include "certradar/ui_model.hpp"

#include <doctest/doctest.h>

TEST_CASE("candidate label explains classification and keeps the local path visible") {
    const certradar::SearchCandidate candidate{
        "C:/Users/Teste/Downloads/certificado.pfx", 100, certradar::CandidateState::recognized};

    const auto label = certradar::format_candidate_label(candidate);

    CHECK(label.find(L"contêiner reconhecido") != std::wstring::npos);
    CHECK(label.find(L"certificado.pfx") != std::wstring::npos);
}

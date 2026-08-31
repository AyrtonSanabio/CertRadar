#pragma once

#include "certradar/search.hpp"

#include <string>

namespace certradar {

enum class CandidateRevealStatus { ready, no_selection, unsafe_path };

struct CandidateRevealPlan {
    CandidateRevealStatus status{CandidateRevealStatus::no_selection};
    std::filesystem::path path;
};

std::wstring candidate_state_label(CandidateState state);
std::wstring format_candidate_label(const SearchCandidate& candidate);
std::wstring build_search_support_summary(const SearchResult& result);
CandidateRevealPlan build_candidate_reveal_plan(
    const SearchResult& result,
    std::size_t selection_index);

}  // namespace certradar

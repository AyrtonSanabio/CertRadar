#pragma once

#include "certradar/platform.hpp"
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
std::wstring build_search_support_summary(
    const SearchResult& result,
    const WindowsPlatform& platform);
CandidateRevealPlan build_candidate_reveal_plan(
    const SearchResult& result,
    std::size_t selection_index);
std::wstring format_platform_summary(const WindowsPlatform& platform);

}  // namespace certradar

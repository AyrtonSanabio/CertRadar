#pragma once

#include "certradar/search.hpp"

#include <string>

namespace certradar {

std::wstring candidate_state_label(CandidateState state);
std::wstring format_candidate_label(const SearchCandidate& candidate);
std::wstring build_search_support_summary(const SearchResult& result);

}  // namespace certradar

#pragma once

#include "certradar/search.hpp"

#include <string>

namespace certradar {

std::wstring candidate_state_label(CandidateState state);
std::wstring format_candidate_label(const SearchCandidate& candidate);

}  // namespace certradar

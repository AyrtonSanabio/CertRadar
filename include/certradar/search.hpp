#pragma once

#include "certradar/facts.hpp"

#include <cstdint>
#include <filesystem>
#include <vector>

namespace certradar {

enum class CandidateState { extension_match, recognized, invalid, too_large, inaccessible };

struct SearchCandidate {
    std::filesystem::path path;
    std::uintmax_t size{0};
    CandidateState state{CandidateState::extension_match};
};

struct SearchOptions {
    bool recursive{false};
    bool validate_containers{false};
    std::uintmax_t maximum_candidate_size{32ULL * 1024ULL * 1024ULL};
};

struct SearchResult {
    ScanStatus status{ScanStatus::not_started};
    std::vector<SearchCandidate> candidates;
    std::uint64_t directories_visited{0};
    std::uint64_t access_denied_count{0};
    std::uint64_t errors{0};
};

bool has_a1_extension(const std::filesystem::path& path);
SearchResult search_files(
    const std::vector<std::filesystem::path>& roots,
    const SearchOptions& options = {});

}  // namespace certradar

#pragma once

#include "certradar/facts.hpp"

#include <cstdint>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <functional>
#include <mutex>
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

struct SearchProgress {
    std::uint64_t directories_visited{0};
    std::uint64_t candidates{0};
};

class SearchControl {
public:
    void request_pause() noexcept;
    void resume() noexcept;
    void cancel() noexcept;
    bool is_paused() const noexcept;
    bool is_cancelled() const noexcept;
    void wait_if_paused();

private:
    std::atomic<bool> paused_{false};
    std::atomic<bool> cancelled_{false};
    std::mutex mutex_;
    std::condition_variable condition_;
};

using ProgressCallback = std::function<void(const SearchProgress&)>;

bool has_a1_extension(const std::filesystem::path& path);
bool should_traverse_directory(std::uint32_t windows_attributes) noexcept;
SearchResult search_files(
    const std::vector<std::filesystem::path>& roots,
    const SearchOptions& options = {});
SearchResult search_files(
    const std::vector<std::filesystem::path>& roots,
    const SearchOptions& options,
    SearchControl& control,
    const ProgressCallback& progress);

}  // namespace certradar

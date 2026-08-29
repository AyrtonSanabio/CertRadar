#include "certradar/search.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>

#include <algorithm>
#include <cwctype>
#include <deque>
#include <fstream>
#include <limits>
#include <vector>

namespace certradar {

void SearchControl::request_pause() noexcept { paused_.store(true); }

void SearchControl::resume() noexcept {
    paused_.store(false);
    condition_.notify_all();
}

void SearchControl::cancel() noexcept {
    cancelled_.store(true);
    condition_.notify_all();
}

bool SearchControl::is_paused() const noexcept { return paused_.load(); }
bool SearchControl::is_cancelled() const noexcept { return cancelled_.load(); }

void SearchControl::wait_if_paused() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this] { return !paused_.load() || cancelled_.load(); });
}

bool has_a1_extension(const std::filesystem::path& path) {
    std::wstring extension = path.extension().wstring();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](const wchar_t value) {
        return static_cast<wchar_t>(std::towlower(value));
    });
    return extension == L".pfx" || extension == L".p12";
}

bool should_traverse_directory(const std::uint32_t windows_attributes) noexcept {
    return (windows_attributes & FILE_ATTRIBUTE_DIRECTORY) != 0 &&
           (windows_attributes & FILE_ATTRIBUTE_REPARSE_POINT) == 0;
}

CandidateState inspect_pkcs12_container(
    const std::filesystem::path& path,
    const std::uintmax_t maximum_size) {
    std::error_code error;
    const auto size = std::filesystem::file_size(path, error);
    if (error) return CandidateState::inaccessible;
    if (size > maximum_size || size > std::numeric_limits<DWORD>::max()) {
        return CandidateState::too_large;
    }

    std::ifstream input(path, std::ios::binary);
    if (!input) return CandidateState::inaccessible;
    std::vector<unsigned char> content(static_cast<std::size_t>(size));
    if (!content.empty()) {
        input.read(reinterpret_cast<char*>(content.data()), static_cast<std::streamsize>(content.size()));
        if (!input) return CandidateState::inaccessible;
    }

    CRYPT_DATA_BLOB blob{};
    blob.cbData = static_cast<DWORD>(content.size());
    blob.pbData = content.empty() ? nullptr : content.data();
    return PFXIsPFXBlob(&blob) != FALSE ? CandidateState::recognized : CandidateState::invalid;
}

SearchResult search_files(
    const std::vector<std::filesystem::path>& roots,
    const SearchOptions& options) {
    SearchControl control;
    return search_files(roots, options, control, {});
}

SearchResult search_files(
    const std::vector<std::filesystem::path>& roots,
    const SearchOptions& options,
    SearchControl& control,
    const ProgressCallback& progress) {
    SearchResult result;
    result.status = ScanStatus::running;

    std::deque<std::filesystem::path> pending(roots.begin(), roots.end());
    while (!pending.empty()) {
        control.wait_if_paused();
        if (control.is_cancelled()) {
            result.status = ScanStatus::cancelled;
            return result;
        }
        const auto root = pending.front();
        pending.pop_front();
        std::error_code error;
        std::filesystem::directory_iterator iterator(
            root, std::filesystem::directory_options::skip_permission_denied, error);
        if (error) {
            if (error == std::errc::permission_denied) ++result.access_denied_count;
            else ++result.errors;
            continue;
        }

        ++result.directories_visited;
        if (progress) progress({result.directories_visited, result.candidates.size()});
        if (control.is_cancelled()) {
            result.status = ScanStatus::cancelled;
            return result;
        }
        std::vector<std::filesystem::directory_entry> entries;
        const std::filesystem::directory_iterator end;
        while (iterator != end) {
            entries.push_back(*iterator);
            iterator.increment(error);
            if (error) {
                if (error == std::errc::permission_denied) ++result.access_denied_count;
                else ++result.errors;
                error.clear();
                break;
            }
        }
        std::sort(entries.begin(), entries.end(), [](const auto& left, const auto& right) {
            return left.path().wstring() < right.path().wstring();
        });

        for (const auto& entry : entries) {
            control.wait_if_paused();
            if (control.is_cancelled()) {
                result.status = ScanStatus::cancelled;
                return result;
            }
            error.clear();
            const DWORD attributes = GetFileAttributesW(entry.path().c_str());
            if (options.recursive && attributes != INVALID_FILE_ATTRIBUTES &&
                should_traverse_directory(attributes)) {
                pending.push_back(entry.path());
                continue;
            }
            error.clear();
            if (!entry.is_regular_file(error) || error || !has_a1_extension(entry.path())) continue;
            const auto size = entry.file_size(error);
            CandidateState state = CandidateState::extension_match;
            if (options.validate_containers) {
                state = inspect_pkcs12_container(entry.path(), options.maximum_candidate_size);
            }
            result.candidates.push_back({
                entry.path(), error ? 0 : size, state});
            if (progress) progress({result.directories_visited, result.candidates.size()});
        }
    }

    result.status = ScanStatus::completed;
    return result;
}

}  // namespace certradar

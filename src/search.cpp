#include "certradar/search.hpp"

#include <algorithm>
#include <cwctype>

namespace certradar {

bool has_a1_extension(const std::filesystem::path& path) {
    std::wstring extension = path.extension().wstring();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](const wchar_t value) {
        return static_cast<wchar_t>(std::towlower(value));
    });
    return extension == L".pfx" || extension == L".p12";
}

SearchResult search_files(
    const std::vector<std::filesystem::path>& roots,
    const SearchOptions& options) {
    SearchResult result;
    result.status = ScanStatus::running;

    for (const auto& root : roots) {
        std::error_code error;
        std::filesystem::directory_iterator iterator(
            root, std::filesystem::directory_options::skip_permission_denied, error);
        if (error) {
            ++result.access_denied_count;
            continue;
        }

        ++result.directories_visited;
        std::vector<std::filesystem::directory_entry> entries;
        for (const auto& entry : iterator) entries.push_back(entry);
        std::sort(entries.begin(), entries.end(), [](const auto& left, const auto& right) {
            return left.path().wstring() < right.path().wstring();
        });

        for (const auto& entry : entries) {
            error.clear();
            if (!entry.is_regular_file(error) || error || !has_a1_extension(entry.path())) continue;
            const auto size = entry.file_size(error);
            result.candidates.push_back({
                entry.path(), error ? 0 : size, CandidateState::extension_match});
        }
    }

    static_cast<void>(options);
    result.status = ScanStatus::completed;
    return result;
}

}  // namespace certradar

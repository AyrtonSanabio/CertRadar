#include "certradar/search_plan.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <knownfolders.h>
#include <shlobj.h>

#include <cwchar>
#include <stdexcept>

namespace certradar {
namespace {

std::filesystem::path current_profile_path() {
    PWSTR raw = nullptr;
    const HRESULT status = SHGetKnownFolderPath(FOLDERID_Profile, KF_FLAG_DEFAULT, nullptr, &raw);
    if (FAILED(status) || raw == nullptr) throw std::runtime_error("Current profile is unavailable");
    const std::filesystem::path result(raw);
    CoTaskMemFree(raw);
    return result;
}

std::vector<std::filesystem::path> sibling_profiles(const std::filesystem::path& current) {
    std::vector<std::filesystem::path> profiles;
    std::error_code error;
    std::filesystem::directory_iterator iterator(
        current.parent_path(), std::filesystem::directory_options::skip_permission_denied, error);
    if (error) return profiles;
    for (const auto& entry : iterator) {
        if (!entry.is_directory(error) || error || entry.path() == current) {
            error.clear();
            continue;
        }
        const DWORD attributes = GetFileAttributesW(entry.path().c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES &&
            (attributes & FILE_ATTRIBUTE_REPARSE_POINT) == 0) {
            profiles.push_back(entry.path());
        }
    }
    return profiles;
}

std::vector<WindowsDrive> windows_drives() {
    const DWORD required = GetLogicalDriveStringsW(0, nullptr);
    if (required == 0) return {};
    std::vector<wchar_t> buffer(static_cast<std::size_t>(required) + 1U, L'\0');
    if (GetLogicalDriveStringsW(required, buffer.data()) == 0) return {};

    std::vector<WindowsDrive> drives;
    const wchar_t* cursor = buffer.data();
    while (*cursor != L'\0') {
        drives.push_back({cursor, GetDriveTypeW(cursor)});
        cursor += std::wcslen(cursor) + 1U;
    }
    return drives;
}

}  // namespace

std::vector<SearchRoot> compose_search_plan(
    const PriorityFolders& priority,
    const std::filesystem::path& current_profile,
    const std::vector<std::filesystem::path>& other_profiles,
    const std::vector<WindowsDrive>& drives) {
    std::vector<SearchRoot> plan;
    for (const auto& path : ordered_priority_folders(priority)) {
        if (!path.empty()) plan.push_back({path, SearchPhase::priority});
    }
    if (!current_profile.empty()) plan.push_back({current_profile, SearchPhase::current_profile});
    for (const auto& profile : other_profiles) {
        if (!profile.empty()) plan.push_back({profile, SearchPhase::other_profiles});
    }
    for (const auto& drive : drives) {
        if (drive.type == DRIVE_FIXED && !drive.root.empty()) {
            plan.push_back({drive.root, SearchPhase::fixed_drives});
        }
    }
    for (const auto& drive : drives) {
        if (drive.type == DRIVE_REMOVABLE && !drive.root.empty()) {
            plan.push_back({drive.root, SearchPhase::removable_drives});
        }
    }
    return plan;
}

std::vector<SearchRoot> build_default_search_plan() {
    const auto profile = current_profile_path();
    return compose_search_plan(
        resolve_priority_folders(), profile, sibling_profiles(profile), windows_drives());
}

std::string search_phase_name(const SearchPhase phase) {
    switch (phase) {
        case SearchPhase::priority: return "priority";
        case SearchPhase::current_profile: return "current_profile";
        case SearchPhase::other_profiles: return "other_profiles";
        case SearchPhase::fixed_drives: return "fixed_drives";
        case SearchPhase::removable_drives: return "removable_drives";
    }
    return "unknown";
}

}  // namespace certradar

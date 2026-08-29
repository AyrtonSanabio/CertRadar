#include "certradar/known_folders.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <knownfolders.h>
#include <shlobj.h>

#include <stdexcept>
#include <string>

namespace certradar {
namespace {

std::filesystem::path resolve_folder(const KNOWNFOLDERID& identifier, const char* label) {
    PWSTR raw_path = nullptr;
    const HRESULT result = SHGetKnownFolderPath(identifier, KF_FLAG_DEFAULT, nullptr, &raw_path);
    if (FAILED(result) || raw_path == nullptr) {
        throw std::runtime_error(std::string("Windows could not resolve ") + label);
    }

    const std::filesystem::path path(raw_path);
    CoTaskMemFree(raw_path);
    return path;
}

}  // namespace

PriorityFolders resolve_priority_folders() {
    return {
        resolve_folder(FOLDERID_Downloads, "Downloads"),
        resolve_folder(FOLDERID_Desktop, "Desktop"),
        resolve_folder(FOLDERID_Documents, "Documents"),
    };
}

std::vector<std::filesystem::path> ordered_priority_folders(const PriorityFolders& folders) {
    return {folders.downloads, folders.desktop, folders.documents};
}

}  // namespace certradar

#pragma once

#include <filesystem>
#include <vector>

namespace certradar {

struct PriorityFolders {
    std::filesystem::path downloads;
    std::filesystem::path desktop;
    std::filesystem::path documents;
};

PriorityFolders resolve_priority_folders();
std::vector<std::filesystem::path> ordered_priority_folders(const PriorityFolders& folders);

}  // namespace certradar

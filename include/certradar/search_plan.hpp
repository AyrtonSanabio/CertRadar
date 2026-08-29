#pragma once

#include "certradar/known_folders.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace certradar {

enum class SearchPhase { priority, current_profile, other_profiles, fixed_drives, removable_drives };

struct SearchRoot {
    std::filesystem::path path;
    SearchPhase phase{SearchPhase::priority};
};

struct WindowsDrive {
    std::filesystem::path root;
    unsigned int type{0};
};

std::vector<SearchRoot> compose_search_plan(
    const PriorityFolders& priority,
    const std::filesystem::path& current_profile,
    const std::vector<std::filesystem::path>& other_profiles,
    const std::vector<WindowsDrive>& drives);
std::vector<SearchRoot> build_default_search_plan();
std::string search_phase_name(SearchPhase phase);

}  // namespace certradar

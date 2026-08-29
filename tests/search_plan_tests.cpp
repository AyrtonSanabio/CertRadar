#include "certradar/search_plan.hpp"

#include <doctest/doctest.h>

TEST_CASE("search plan puts profiles before fixed drives and excludes network drives") {
    constexpr unsigned int drive_fixed = 3;
    constexpr unsigned int drive_remote = 4;
    const certradar::PriorityFolders priority{"P:/Downloads", "P:/Desktop", "P:/Documents"};
    const std::vector<std::filesystem::path> others{"C:/Users/Ana", "C:/Users/Beto"};
    const std::vector<certradar::WindowsDrive> drives{
        {"C:/", drive_fixed}, {"Z:/", drive_remote}};

    const auto plan = certradar::compose_search_plan(priority, "C:/Users/Atual", others, drives);

    REQUIRE(plan.size() == 7);
    CHECK(plan[0].path == priority.downloads);
    CHECK(plan[3].phase == certradar::SearchPhase::current_profile);
    CHECK(plan[4].phase == certradar::SearchPhase::other_profiles);
    CHECK(plan[5].phase == certradar::SearchPhase::other_profiles);
    CHECK(plan[6].path == std::filesystem::path("C:/"));
    CHECK(plan[6].phase == certradar::SearchPhase::fixed_drives);
}

TEST_CASE("search plan appends removable drives but still excludes optical and remote media") {
    constexpr unsigned int drive_removable = 2;
    constexpr unsigned int drive_remote = 4;
    constexpr unsigned int drive_cdrom = 5;
    const certradar::PriorityFolders priority{"P:/D", "P:/A", "P:/Docs"};
    const std::vector<certradar::WindowsDrive> drives{
        {"E:/", drive_removable}, {"Z:/", drive_remote}, {"F:/", drive_cdrom}};

    const auto plan = certradar::compose_search_plan(priority, {}, {}, drives);

    REQUIRE(plan.size() == 4);
    CHECK(plan.back().path == std::filesystem::path("E:/"));
    CHECK(plan.back().phase == certradar::SearchPhase::removable_drives);
}

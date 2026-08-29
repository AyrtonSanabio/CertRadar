#include "certradar/known_folders.hpp"

#include <doctest/doctest.h>

TEST_CASE("priority folders always keep support-oriented order") {
    const certradar::PriorityFolders folders{"D:/Baixados", "D:/Mesa", "D:/Documentos"};
    const auto ordered = certradar::ordered_priority_folders(folders);

    REQUIRE(ordered.size() == 3);
    CHECK(ordered[0] == folders.downloads);
    CHECK(ordered[1] == folders.desktop);
    CHECK(ordered[2] == folders.documents);
}

TEST_CASE("priority folders are resolved from Windows") {
    const auto folders = certradar::resolve_priority_folders();
    CHECK_FALSE(folders.downloads.empty());
    CHECK_FALSE(folders.desktop.empty());
    CHECK_FALSE(folders.documents.empty());
}

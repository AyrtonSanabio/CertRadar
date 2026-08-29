#include "certradar/platform.hpp"

#include <doctest/doctest.h>

TEST_CASE("Windows detector returns observable platform facts") {
    const auto platform = certradar::detect_windows_platform();

    CHECK(platform.major >= 5);
    CHECK(platform.build > 0);
    CHECK((platform.architecture == "x86" || platform.architecture == "x64" ||
           platform.architecture == "arm64" || platform.architecture == "unknown"));

    const auto facts = certradar::to_platform_facts(platform);
    CHECK(facts.family == "windows");
    CHECK(facts.version_major == platform.major);
    CHECK(facts.version_minor == platform.minor);
    CHECK(facts.build == platform.build);
    CHECK(facts.architecture == platform.architecture);
}

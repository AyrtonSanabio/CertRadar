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

TEST_CASE("support classifier follows the documented Windows matrix") {
    using certradar::SupportMode;
    using certradar::WindowsPlatform;

    CHECK(certradar::classify_support_mode(WindowsPlatform{10, 0, 19045}) == SupportMode::full);
    CHECK(certradar::classify_support_mode(WindowsPlatform{10, 0, 26100}) == SupportMode::full);
    CHECK(certradar::classify_support_mode(WindowsPlatform{6, 3, 9600}) == SupportMode::compatible);

    WindowsPlatform windows_7_sp1{6, 1, 7601};
    windows_7_sp1.service_pack_major = 1;
    CHECK(certradar::classify_support_mode(windows_7_sp1) == SupportMode::compatible);

    WindowsPlatform windows_7_without_sp1{6, 1, 7600};
    CHECK(certradar::classify_support_mode(windows_7_without_sp1) == SupportMode::unsupported);

    WindowsPlatform xp_sp3{5, 1, 2600};
    xp_sp3.service_pack_major = 3;
    CHECK(certradar::classify_support_mode(xp_sp3) == SupportMode::legacy);

    CHECK(certradar::classify_support_mode(WindowsPlatform{6, 2, 9200}) == SupportMode::unsupported);
    CHECK(certradar::classify_support_mode(WindowsPlatform{6, 0, 6002}) == SupportMode::unsupported);
}

TEST_CASE("platform facts include the classified support mode") {
    certradar::WindowsPlatform windows_81{6, 3, 9600};
    CHECK(certradar::to_platform_facts(windows_81).mode == certradar::SupportMode::compatible);
}

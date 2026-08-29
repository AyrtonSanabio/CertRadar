#include "certradar/facts.hpp"

#include <doctest/doctest.h>
#include <nlohmann/json.hpp>

TEST_CASE("fact document serializes a stable versioned contract") {
    certradar::FactDocument document;
    document.platform.version_major = 10;
    document.platform.build = 19045;
    document.platform.architecture = "x64";
    document.platform.mode = certradar::SupportMode::full;
    document.scan.status = certradar::ScanStatus::completed;

    const auto parsed = nlohmann::json::parse(certradar::serialize_facts(document));

    CHECK(parsed.at("schemaVersion") == 1);
    CHECK(parsed.at("platform").at("family") == "windows");
    CHECK(parsed.at("platform").at("mode") == "full");
    CHECK(parsed.at("scan").at("status") == "completed");
    CHECK(parsed.at("errors").empty());
}

TEST_CASE("fact document round trip preserves supported fields") {
    certradar::FactDocument expected;
    expected.platform.version_major = 6;
    expected.platform.version_minor = 1;
    expected.platform.service_pack = "Service Pack 1";
    expected.platform.mode = certradar::SupportMode::compatible;
    expected.scan.status = certradar::ScanStatus::partial;
    expected.scan.access_denied_count = 3;
    expected.errors.push_back({"access_denied", "filesystem", "A folder could not be read."});

    const auto actual = certradar::deserialize_facts(certradar::serialize_facts(expected));

    CHECK(actual.schema_version == certradar::FactDocument::current_schema_version);
    CHECK(actual.platform.version_major == 6);
    CHECK(actual.platform.version_minor == 1);
    CHECK(actual.platform.service_pack == "Service Pack 1");
    CHECK(actual.platform.mode == certradar::SupportMode::compatible);
    CHECK(actual.scan.status == certradar::ScanStatus::partial);
    CHECK(actual.scan.access_denied_count == 3);
    REQUIRE(actual.errors.size() == 1);
    CHECK(actual.errors.front().code == "access_denied");
}

TEST_CASE("unsupported fact schema is rejected") {
    CHECK_THROWS_WITH_AS(
        certradar::deserialize_facts(R"({"schemaVersion":2})"),
        "Unsupported facts schema version: 2",
        std::invalid_argument);
}

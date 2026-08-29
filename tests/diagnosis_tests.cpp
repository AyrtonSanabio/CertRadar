#include "certradar/diagnosis.hpp"

#include <doctest/doctest.h>

TEST_CASE("diagnosis explains public certificate without private key") {
    certradar::DiagnosticInput input;
    input.certificate_installed = true;
    input.reader_detected = true;
    input.device_present = true;
    const auto findings = certradar::diagnose(input);
    REQUIRE(findings.size() == 1);
    CHECK(findings.front().id == "certificate_without_private_key");
    CHECK(findings.front().confidence == certradar::Confidence::high);
    CHECK_FALSE(findings.front().fact.empty());
    CHECK_FALSE(findings.front().probable_cause.empty());
    CHECK_FALSE(findings.front().recommended_action.empty());
}

TEST_CASE("diagnosis admits insufficient evidence instead of inventing a cause") {
    certradar::DiagnosticInput input;
    input.reader_detected = true;
    input.device_present = true;
    const auto findings = certradar::diagnose(input);
    REQUIRE(findings.size() == 1);
    CHECK(findings.front().id == "insufficient_evidence");
    CHECK(findings.front().confidence == certradar::Confidence::low);
}

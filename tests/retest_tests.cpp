#include "certradar/retest.hpp"

#include <doctest/doctest.h>

TEST_CASE("retest plan contains only checks affected by the authorized action") {
    CHECK(certradar::affected_checks_for_action("smartcard_service_start") ==
          std::vector<std::string>{"smartcard_service", "smartcard_reader", "smartcard_device"});
    CHECK(certradar::affected_checks_for_action("certificate_import_wizard") ==
          std::vector<std::string>{"certificate_store", "private_key_association"});
    CHECK(certradar::affected_checks_for_action("official_middleware_page") ==
          std::vector<std::string>{"middleware_provider"});
    CHECK(certradar::affected_checks_for_action("arbitrary_command").empty());
}

TEST_CASE("before and after comparison ignores unrelated facts and classifies changes") {
    using certradar::RetestObservation;
    using certradar::RetestOutcome;
    using certradar::RetestState;

    const std::vector<RetestObservation> before{
        {"smartcard_service", RetestState::failing, "parado"},
        {"smartcard_reader", RetestState::failing, "nenhum leitor"},
        {"smartcard_device", RetestState::passing, "presente"},
        {"unrelated_application", RetestState::passing, "ok"},
    };
    const std::vector<RetestObservation> after{
        {"smartcard_service", RetestState::passing, "em execucao"},
        {"smartcard_reader", RetestState::failing, "nenhum leitor"},
        {"smartcard_device", RetestState::failing, "ausente"},
        {"unrelated_application", RetestState::failing, "falhou"},
    };

    const auto comparison = certradar::compare_affected_retest(
        "smartcard_service_start", before, after);
    REQUIRE(comparison.size() == 3);
    CHECK(comparison[0].check_id == "smartcard_service");
    CHECK(comparison[0].outcome == RetestOutcome::resolved);
    CHECK(comparison[1].outcome == RetestOutcome::unchanged);
    CHECK(comparison[2].outcome == RetestOutcome::regressed);
}

TEST_CASE("missing retest evidence is explicit instead of being treated as success") {
    using certradar::RetestObservation;
    using certradar::RetestOutcome;
    using certradar::RetestState;

    const std::vector<RetestObservation> before{
        {"certificate_store", RetestState::failing, "ausente"},
    };
    const std::vector<RetestObservation> after{
        {"certificate_store", RetestState::passing, "presente"},
    };
    const auto comparison = certradar::compare_affected_retest(
        "certificate_import_wizard", before, after);
    REQUIRE(comparison.size() == 2);
    CHECK(comparison[0].outcome == RetestOutcome::resolved);
    CHECK(comparison[1].check_id == "private_key_association");
    CHECK(comparison[1].outcome == RetestOutcome::incomplete);
}

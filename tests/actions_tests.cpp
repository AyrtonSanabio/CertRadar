#include "certradar/actions.hpp"

#include <doctest/doctest.h>

TEST_CASE("executor policy refuses unknown actions and missing consent") {
    CHECK(certradar::authorize_action("cmd /c qualquer-coisa", true) ==
          certradar::ActionAuthorization::unknown_action);
    CHECK(certradar::authorize_action("smartcard_service_start", false) ==
          certradar::ActionAuthorization::consent_required);
    CHECK(certradar::authorize_action("smartcard_service_start", true) ==
          certradar::ActionAuthorization::allowed);
}

TEST_CASE("every allowed action has a stable id and consent description") {
    const auto& actions = certradar::allowed_actions();
    REQUIRE(actions.size() == 3);
    for (const auto& action : actions) {
        CHECK_FALSE(action.id.empty());
        CHECK_FALSE(action.consent_description.empty());
        CHECK(action.id.find(' ') == std::string::npos);
    }
}

TEST_CASE("smart card service action is planned without mutating the test machine") {
    using certradar::ServiceActionPlan;
    using certradar::ServiceState;

    CHECK(certradar::plan_smartcard_service_start(ServiceState::stopped, false) ==
          ServiceActionPlan::blocked);
    CHECK(certradar::plan_smartcard_service_start(ServiceState::running, true) ==
          ServiceActionPlan::no_change);
    CHECK(certradar::plan_smartcard_service_start(ServiceState::stopped, true) ==
          ServiceActionPlan::start);
    CHECK(certradar::plan_smartcard_service_start(ServiceState::inaccessible, true) ==
          ServiceActionPlan::start);
}

TEST_CASE("import wizard only opens for a recognized container with consent") {
    using certradar::CandidateState;
    using certradar::ImportActionPlan;

    CHECK(certradar::plan_certificate_import(CandidateState::recognized, false) ==
          ImportActionPlan::blocked);
    CHECK(certradar::plan_certificate_import(CandidateState::invalid, true) ==
          ImportActionPlan::blocked);
    CHECK(certradar::plan_certificate_import(CandidateState::too_large, true) ==
          ImportActionPlan::blocked);
    CHECK(certradar::plan_certificate_import(CandidateState::recognized, true) ==
          ImportActionPlan::open_wizard);
}

TEST_CASE("middleware source is selected from a closed provider registry") {
    const auto* safesign = certradar::find_middleware_source("SafeSign Standard Cryptographic Service Provider");
    REQUIRE(safesign != nullptr);
    CHECK(safesign->id == "safesign");
    CHECK(safesign->official_url.rfind("https://", 0) == 0);

    const auto* etoken = certradar::find_middleware_source("eToken Base Cryptographic Provider");
    REQUIRE(etoken != nullptr);
    CHECK(etoken->id == "etoken");
    CHECK(certradar::find_middleware_source("Provider desconhecido") == nullptr);
}

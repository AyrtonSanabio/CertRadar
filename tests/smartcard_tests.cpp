#include "certradar/smartcard.hpp"

#include <doctest/doctest.h>

TEST_CASE("smart card service query always returns an explainable state") {
    const auto service = certradar::query_smart_card_service();
    CHECK((service.state != certradar::ServiceState::unknown || service.native_state != 0));
    if (service.state == certradar::ServiceState::inaccessible ||
        service.state == certradar::ServiceState::missing) {
        CHECK(service.error_code != 0);
    }
}

TEST_CASE("reader enumeration does not require a card or consume a PIN attempt") {
    const auto result = certradar::enumerate_smart_card_readers();
    CHECK((result.success || result.error_code != 0));
    for (const auto& reader : result.readers) CHECK_FALSE(reader.name.empty());
}

TEST_CASE("A3 diagnosis keeps service reader device middleware and key as separate states") {
    certradar::A3Evidence evidence;
    CHECK(certradar::diagnose_a3_state(evidence) == certradar::A3State::service_unavailable);
    evidence.service = certradar::ServiceState::running;
    CHECK(certradar::diagnose_a3_state(evidence) == certradar::A3State::reader_missing);
    evidence.reader_detected = true;
    CHECK(certradar::diagnose_a3_state(evidence) == certradar::A3State::device_absent);
    evidence.device_present = true;
    CHECK(certradar::diagnose_a3_state(evidence) == certradar::A3State::middleware_missing);
    evidence.provider_detected = true;
    CHECK(certradar::diagnose_a3_state(evidence) == certradar::A3State::key_unavailable);
    evidence.key_associated = true;
    CHECK(certradar::diagnose_a3_state(evidence) == certradar::A3State::apparently_ready);
}

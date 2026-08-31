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

TEST_CASE("A3 local inspection only queries readers when the service is running") {
    const auto snapshot = certradar::inspect_a3_locally();

    CHECK(snapshot.readers_queried ==
          (snapshot.service.state == certradar::ServiceState::running));
    if (snapshot.readers_queried) {
        CHECK((snapshot.readers.success || snapshot.readers.error_code != 0));
    } else {
        CHECK(snapshot.readers.readers.empty());
    }
}

TEST_CASE("A3 local triage preserves each observable support boundary") {
    certradar::A3LocalSnapshot snapshot;
    CHECK(certradar::classify_a3_local_snapshot(snapshot) ==
          certradar::A3LocalTriage::service_unavailable);

    snapshot.service.state = certradar::ServiceState::running;
    snapshot.readers_queried = true;
    CHECK(certradar::classify_a3_local_snapshot(snapshot) ==
          certradar::A3LocalTriage::reader_query_failed);

    snapshot.readers.success = true;
    CHECK(certradar::classify_a3_local_snapshot(snapshot) ==
          certradar::A3LocalTriage::reader_missing);

    snapshot.readers.readers.push_back({L"Leitor", false, false, 0});
    CHECK(certradar::classify_a3_local_snapshot(snapshot) ==
          certradar::A3LocalTriage::device_absent);

    snapshot.readers.readers[0].card_present = true;
    snapshot.readers.readers[0].unavailable = true;
    CHECK(certradar::classify_a3_local_snapshot(snapshot) ==
          certradar::A3LocalTriage::device_absent);

    snapshot.readers.readers[0].unavailable = false;
    CHECK(certradar::classify_a3_local_snapshot(snapshot) ==
          certradar::A3LocalTriage::device_detected);
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

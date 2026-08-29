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

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

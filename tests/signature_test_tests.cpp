#include "certradar/signature_test.hpp"

#include <doctest/doctest.h>

TEST_CASE("synthetic A1 challenge is signed and verified without persisted credentials") {
    const auto result = certradar::run_synthetic_a1_signature_test();
    INFO(result.message);
    CHECK(result.signed_and_verified);
    CHECK(result.native_status >= 0);
}

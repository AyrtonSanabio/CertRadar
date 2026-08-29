#include "certradar/signature_test.hpp"

#include <doctest/doctest.h>

TEST_CASE("synthetic A1 challenge is signed and verified without persisted credentials") {
    const auto result = certradar::run_synthetic_a1_signature_test();
    INFO(result.message);
    CHECK(result.signed_and_verified);
    CHECK(result.native_status >= 0);
}

TEST_CASE("external token operation is blocked until explicit consent") {
    CHECK(certradar::external_signature_permission(false) ==
          certradar::ExternalSignaturePermission::blocked);
    CHECK(certradar::external_signature_permission(true) ==
          certradar::ExternalSignaturePermission::authorized);
}

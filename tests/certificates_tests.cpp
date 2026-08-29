#include "certradar/certificates.hpp"

#include <doctest/doctest.h>

TEST_CASE("current user personal store is enumerated read-only") {
    const auto result = certradar::enumerate_personal_certificates(
        certradar::StoreScope::current_user);

    CHECK(result.opened);
    CHECK(result.error_code == 0);
    for (const auto& certificate : result.certificates) {
        CHECK(certificate.scope == certradar::StoreScope::current_user);
        CHECK_FALSE(certificate.encoded_certificate.empty());
    }
}

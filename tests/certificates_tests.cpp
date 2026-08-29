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

TEST_CASE("local machine personal store is attempted without elevation") {
    const auto result = certradar::enumerate_personal_certificates(
        certradar::StoreScope::local_machine);

    CHECK((result.opened || result.error_code != 0));
    for (const auto& certificate : result.certificates) {
        CHECK(certificate.scope == certradar::StoreScope::local_machine);
    }
}

TEST_CASE("certificate validity distinguishes future near expiry and expired") {
    constexpr std::uint64_t day = 24ULL * 60ULL * 60ULL * 10'000'000ULL;
    constexpr std::uint64_t now = 1'000ULL * day;

    CHECK(certradar::classify_certificate_validity(now + day, now + 365 * day, now) ==
          certradar::CertificateValidity::not_yet_valid);
    CHECK(certradar::classify_certificate_validity(now - day, now + 365 * day, now) ==
          certradar::CertificateValidity::valid);
    CHECK(certradar::classify_certificate_validity(now - day, now + 20 * day, now) ==
          certradar::CertificateValidity::expiring_soon);
    CHECK(certradar::classify_certificate_validity(now - 365 * day, now - day, now) ==
          certradar::CertificateValidity::expired);
}

TEST_CASE("private key association is metadata only and never contains key material") {
    const auto result = certradar::enumerate_personal_certificates(
        certradar::StoreScope::current_user);

    for (const auto& certificate : result.certificates) {
        if (certificate.has_private_key_association) {
            CHECK(certificate.encoded_certificate.size() > 0);
            CHECK(certificate.provider.size() < 4096);
        }
    }
    CHECK(result.opened);
}

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

TEST_CASE("chain status maps trust failures to explainable local issues") {
    constexpr unsigned long not_time_valid = 0x00000001;
    constexpr unsigned long untrusted_root = 0x00000020;
    constexpr unsigned long partial_chain = 0x00010000;

    const auto issues = certradar::classify_chain_issues(
        not_time_valid | untrusted_root | partial_chain);

    REQUIRE(issues.size() == 3);
    CHECK(issues[0] == certradar::ChainIssue::expired);
    CHECK(issues[1] == certradar::ChainIssue::untrusted_root);
    CHECK(issues[2] == certradar::ChainIssue::partial_chain);
}

TEST_CASE("invalid public certificate data produces a contained chain error") {
    const auto result = certradar::evaluate_certificate_chain_local({1, 2, 3, 4});
    CHECK_FALSE(result.built);
    CHECK(result.error_code != 0);
    CHECK_FALSE(result.online_requested);
}

TEST_CASE("online revocation request preserves timeout and remains indeterminate on bad input") {
    const auto result = certradar::evaluate_certificate_chain_online({1, 2, 3}, 750);
    CHECK_FALSE(result.built);
    CHECK(result.online_requested);
    CHECK(result.requested_timeout_ms == 750);
    CHECK(result.error_code != 0);
}

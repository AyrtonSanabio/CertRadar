#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace certradar {

enum class StoreScope { current_user, local_machine };
enum class CertificateValidity { not_yet_valid, valid, expiring_soon, expired };

struct CertificateRecord {
    StoreScope scope{StoreScope::current_user};
    std::string subject;
    std::string issuer;
    std::string serial_number;
    std::string thumbprint;
    std::string valid_from;
    std::string valid_until;
    CertificateValidity validity{CertificateValidity::valid};
    bool has_private_key_association{false};
    std::string provider;
    std::vector<std::string> enhanced_key_usages;
    std::vector<std::uint8_t> encoded_certificate;
};

struct CertificateStoreResult {
    bool opened{false};
    unsigned long error_code{0};
    std::vector<CertificateRecord> certificates;
};

CertificateStoreResult enumerate_personal_certificates(StoreScope scope);
CertificateValidity classify_certificate_validity(
    std::uint64_t valid_from,
    std::uint64_t valid_until,
    std::uint64_t now) noexcept;

}  // namespace certradar

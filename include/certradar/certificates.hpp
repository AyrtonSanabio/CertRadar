#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace certradar {

enum class StoreScope { current_user, local_machine };
enum class CertificateValidity { not_yet_valid, valid, expiring_soon, expired };
enum class ProviderKind { csp, ksp, unknown };

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
    ProviderKind provider_kind{ProviderKind::unknown};
    std::vector<std::string> enhanced_key_usages;
    std::vector<std::uint8_t> encoded_certificate;
};

struct CertificateStoreResult {
    StoreScope scope{StoreScope::current_user};
    bool opened{false};
    unsigned long error_code{0};
    std::vector<CertificateRecord> certificates;
};

enum class ChainIssue {
    untrusted_root,
    partial_chain,
    expired,
    revoked,
    revocation_unknown,
    invalid_basic_constraints,
    other
};

struct ChainEvaluation {
    bool built{false};
    bool online_requested{false};
    unsigned long trust_status{0};
    unsigned long error_code{0};
    std::uint32_t requested_timeout_ms{0};
    std::vector<ChainIssue> issues;
};

CertificateStoreResult enumerate_personal_certificates(StoreScope scope);
CertificateValidity classify_certificate_validity(
    std::uint64_t valid_from,
    std::uint64_t valid_until,
    std::uint64_t now) noexcept;
std::vector<ChainIssue> classify_chain_issues(unsigned long trust_status);
ChainEvaluation evaluate_certificate_chain_local(const std::vector<std::uint8_t>& encoded_certificate);
ChainEvaluation evaluate_certificate_chain_online(
    const std::vector<std::uint8_t>& encoded_certificate,
    std::uint32_t timeout_ms = 5000);
ProviderKind classify_provider_kind(unsigned long provider_type, const std::string& provider_name) noexcept;

}  // namespace certradar

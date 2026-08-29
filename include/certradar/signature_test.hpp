#pragma once

#include <string>

namespace certradar {

struct SignatureTestResult {
    bool signed_and_verified{false};
    long native_status{0};
    std::string message;
};

enum class ExternalSignaturePermission { blocked, authorized };

SignatureTestResult run_synthetic_a1_signature_test();
ExternalSignaturePermission external_signature_permission(bool explicit_user_consent) noexcept;

}  // namespace certradar

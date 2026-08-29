#pragma once

#include <string>

namespace certradar {

struct SignatureTestResult {
    bool signed_and_verified{false};
    long native_status{0};
    std::string message;
};

SignatureTestResult run_synthetic_a1_signature_test();

}  // namespace certradar

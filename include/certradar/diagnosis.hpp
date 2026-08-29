#pragma once

#include <string>
#include <vector>

namespace certradar {

enum class Confidence { low, medium, high };

struct DiagnosticInput {
    bool a1_file_found{false};
    bool certificate_installed{false};
    bool private_key_associated{false};
    bool smartcard_service_running{true};
    bool reader_detected{false};
    bool device_present{false};
};

struct Finding {
    std::string id;
    std::string fact;
    std::string probable_cause;
    Confidence confidence{Confidence::low};
    std::string recommended_action;
};

std::vector<Finding> diagnose(const DiagnosticInput& input);

}  // namespace certradar

#pragma once

#include "certradar/certificates.hpp"
#include "certradar/smartcard.hpp"

#include <string>
#include <vector>

namespace certradar {

enum class EvidenceState { unknown, absent, present };
enum class ProfileCheckStatus { passed, warning, blocked, unknown };
enum class ProfileStatus { ready, attention, blocked, indeterminate };

struct ApplicationProfile {
    std::string id;
    std::string display_name;
    std::string official_documentation_url;
};

struct ProfileCheck {
    std::string id;
    ProfileCheckStatus status{ProfileCheckStatus::unknown};
    std::string fact;
    std::string recommended_action;
};

struct ProfileEvaluation {
    std::string profile_id;
    ProfileStatus status{ProfileStatus::indeterminate};
    std::vector<ProfileCheck> checks;
};

struct PjeProfileInput {
    EvidenceState certificate{EvidenceState::unknown};
    CertificateValidity certificate_validity{CertificateValidity::valid};
    EvidenceState private_key{EvidenceState::unknown};
    EvidenceState trusted_chain{EvidenceState::unknown};
    EvidenceState pjeoffice_installed{EvidenceState::unknown};
    EvidenceState pjeoffice_running{EvidenceState::unknown};
    bool uses_external_device{false};
    A3State external_device_state{A3State::apparently_ready};
};

const std::vector<ApplicationProfile>& application_profiles();
const ApplicationProfile* find_application_profile(const std::string& id);
ProfileEvaluation evaluate_pje_profile(const PjeProfileInput& input);

}  // namespace certradar

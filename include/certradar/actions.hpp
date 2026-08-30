#pragma once

#include "certradar/smartcard.hpp"

#include <string>
#include <vector>

namespace certradar {

enum class ActionAuthorization { allowed, unknown_action, consent_required };
enum class ServiceActionPlan { blocked, no_change, start };
enum class ActionExecutionState { blocked, no_change, applied, failed };

struct AllowedAction {
    std::string id;
    std::string consent_description;
    bool changes_system_state{false};
    bool may_require_elevation{false};
};

struct ServiceActionResult {
    ActionExecutionState state{ActionExecutionState::blocked};
    ServiceState before{ServiceState::unknown};
    ServiceState after{ServiceState::unknown};
    unsigned long error_code{0};
};

const std::vector<AllowedAction>& allowed_actions();
const AllowedAction* find_allowed_action(const std::string& id) noexcept;
ActionAuthorization authorize_action(const std::string& id, bool explicit_consent) noexcept;
ServiceActionPlan plan_smartcard_service_start(
    ServiceState current_state,
    bool explicit_consent) noexcept;
ServiceActionResult execute_smartcard_service_start(bool explicit_consent);

}  // namespace certradar

#pragma once

#include <string>
#include <vector>

namespace certradar {

enum class ActionAuthorization { allowed, unknown_action, consent_required };

struct AllowedAction {
    std::string id;
    std::string consent_description;
    bool changes_system_state{false};
    bool may_require_elevation{false};
};

const std::vector<AllowedAction>& allowed_actions();
const AllowedAction* find_allowed_action(const std::string& id) noexcept;
ActionAuthorization authorize_action(const std::string& id, bool explicit_consent) noexcept;

}  // namespace certradar

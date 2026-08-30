#pragma once

#include "certradar/smartcard.hpp"
#include "certradar/search.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace certradar {

enum class ActionAuthorization { allowed, unknown_action, consent_required };
enum class ServiceActionPlan { blocked, no_change, start };
enum class ActionExecutionState { blocked, no_change, applied, failed };
enum class ImportActionPlan { blocked, open_wizard };

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

struct ImportActionResult {
    ActionExecutionState state{ActionExecutionState::blocked};
    unsigned long error_code{0};
};

struct MiddlewareSource {
    std::string id;
    std::string provider_marker;
    std::string official_url;
};

const std::vector<AllowedAction>& allowed_actions();
const AllowedAction* find_allowed_action(const std::string& id);
ActionAuthorization authorize_action(const std::string& id, bool explicit_consent);
ServiceActionPlan plan_smartcard_service_start(
    ServiceState current_state,
    bool explicit_consent) noexcept;
ServiceActionResult execute_smartcard_service_start(bool explicit_consent);
ImportActionPlan plan_certificate_import(
    CandidateState candidate_state,
    bool explicit_consent) noexcept;
ImportActionResult open_certificate_import_wizard(
    const std::filesystem::path& candidate,
    bool explicit_consent,
    void* parent_window = nullptr);
const std::vector<MiddlewareSource>& middleware_sources();
const MiddlewareSource* find_middleware_source(const std::string& provider_name);
ImportActionResult open_official_middleware_page(
    const std::string& provider_name,
    bool explicit_consent,
    void* parent_window = nullptr);

}  // namespace certradar

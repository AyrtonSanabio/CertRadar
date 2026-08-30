#include "certradar/retest.hpp"

#include <algorithm>

namespace certradar {
namespace {

const RetestObservation* find_observation(
    const std::vector<RetestObservation>& observations,
    const std::string& check_id) noexcept {
    const auto found = std::find_if(
        observations.begin(), observations.end(), [&](const RetestObservation& observation) {
            return observation.check_id == check_id;
        });
    return found == observations.end() ? nullptr : &*found;
}

RetestOutcome classify_outcome(
    const RetestObservation* const before,
    const RetestObservation* const after) noexcept {
    if (before == nullptr || after == nullptr ||
        before->state == RetestState::not_observed ||
        after->state == RetestState::not_observed) {
        return RetestOutcome::incomplete;
    }
    if (before->state == after->state) return RetestOutcome::unchanged;
    if (before->state == RetestState::failing && after->state == RetestState::passing) {
        return RetestOutcome::resolved;
    }
    if (before->state == RetestState::passing && after->state == RetestState::failing) {
        return RetestOutcome::regressed;
    }
    return RetestOutcome::changed;
}

}  // namespace

std::vector<std::string> affected_checks_for_action(const std::string& action_id) {
    if (action_id == "smartcard_service_start") {
        return {"smartcard_service", "smartcard_reader", "smartcard_device"};
    }
    if (action_id == "certificate_import_wizard") {
        return {"certificate_store", "private_key_association"};
    }
    if (action_id == "official_middleware_page") return {"middleware_provider"};
    return {};
}

std::vector<RetestComparison> compare_affected_retest(
    const std::string& action_id,
    const std::vector<RetestObservation>& before,
    const std::vector<RetestObservation>& after) {
    std::vector<RetestComparison> comparisons;
    for (const auto& check_id : affected_checks_for_action(action_id)) {
        const auto* const old_observation = find_observation(before, check_id);
        const auto* const new_observation = find_observation(after, check_id);
        comparisons.push_back({
            check_id,
            old_observation == nullptr ? RetestState::not_observed : old_observation->state,
            new_observation == nullptr ? RetestState::not_observed : new_observation->state,
            classify_outcome(old_observation, new_observation),
        });
    }
    return comparisons;
}

}  // namespace certradar

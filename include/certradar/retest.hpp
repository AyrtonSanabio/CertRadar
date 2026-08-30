#pragma once

#include <string>
#include <vector>

namespace certradar {

enum class RetestState { not_observed, failing, passing };
enum class RetestOutcome { unchanged, resolved, regressed, changed, incomplete };

struct RetestObservation {
    std::string check_id;
    RetestState state{RetestState::not_observed};
    std::string detail;
};

struct RetestComparison {
    std::string check_id;
    RetestState before{RetestState::not_observed};
    RetestState after{RetestState::not_observed};
    RetestOutcome outcome{RetestOutcome::incomplete};
};

std::vector<std::string> affected_checks_for_action(const std::string& action_id);
std::vector<RetestComparison> compare_affected_retest(
    const std::string& action_id,
    const std::vector<RetestObservation>& before,
    const std::vector<RetestObservation>& after);

}  // namespace certradar

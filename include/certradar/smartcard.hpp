#pragma once

#include <string>
#include <vector>

namespace certradar {

enum class ServiceState { running, stopped, pending, missing, inaccessible, unknown };

struct SmartCardService {
    ServiceState state{ServiceState::unknown};
    unsigned long native_state{0};
    unsigned long error_code{0};
};

SmartCardService query_smart_card_service();

}  // namespace certradar

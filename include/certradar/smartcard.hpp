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

struct SmartCardReader {
    std::wstring name;
    bool card_present{false};
    bool unavailable{false};
    unsigned long state{0};
};

struct ReaderEnumeration {
    bool success{false};
    long error_code{0};
    std::vector<SmartCardReader> readers;
};

SmartCardService query_smart_card_service();
ReaderEnumeration enumerate_smart_card_readers();

}  // namespace certradar

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

struct A3LocalSnapshot {
    SmartCardService service;
    bool readers_queried{false};
    ReaderEnumeration readers;
};

enum class A3LocalTriage {
    service_unavailable,
    reader_query_failed,
    reader_missing,
    device_absent,
    device_detected
};

enum class A3State {
    service_unavailable,
    reader_missing,
    device_absent,
    middleware_missing,
    key_unavailable,
    apparently_ready
};

struct A3Evidence {
    ServiceState service{ServiceState::unknown};
    bool reader_detected{false};
    bool device_present{false};
    bool provider_detected{false};
    bool key_associated{false};
};

SmartCardService query_smart_card_service();
ReaderEnumeration enumerate_smart_card_readers();
A3LocalSnapshot inspect_a3_locally();
A3LocalTriage classify_a3_local_snapshot(const A3LocalSnapshot& snapshot) noexcept;
A3State diagnose_a3_state(const A3Evidence& evidence) noexcept;

}  // namespace certradar

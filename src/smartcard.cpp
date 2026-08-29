#include "certradar/smartcard.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace certradar {

SmartCardService query_smart_card_service() {
    SmartCardService result;
    SC_HANDLE manager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (manager == nullptr) {
        result.state = ServiceState::inaccessible;
        result.error_code = GetLastError();
        return result;
    }
    SC_HANDLE service = OpenServiceW(manager, L"SCardSvr", SERVICE_QUERY_STATUS);
    if (service == nullptr) {
        result.error_code = GetLastError();
        result.state = result.error_code == ERROR_SERVICE_DOES_NOT_EXIST
            ? ServiceState::missing : ServiceState::inaccessible;
        CloseServiceHandle(manager);
        return result;
    }

    SERVICE_STATUS_PROCESS status{};
    DWORD received = 0;
    if (QueryServiceStatusEx(
            service, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE>(&status),
            sizeof(status), &received) == FALSE) {
        result.state = ServiceState::inaccessible;
        result.error_code = GetLastError();
    } else {
        result.native_state = status.dwCurrentState;
        switch (status.dwCurrentState) {
            case SERVICE_RUNNING: result.state = ServiceState::running; break;
            case SERVICE_STOPPED: result.state = ServiceState::stopped; break;
            case SERVICE_START_PENDING:
            case SERVICE_STOP_PENDING:
            case SERVICE_PAUSE_PENDING:
            case SERVICE_CONTINUE_PENDING: result.state = ServiceState::pending; break;
            default: result.state = ServiceState::unknown; break;
        }
    }
    CloseServiceHandle(service);
    CloseServiceHandle(manager);
    return result;
}

}  // namespace certradar

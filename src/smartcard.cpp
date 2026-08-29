#include "certradar/smartcard.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winscard.h>

#include <cwchar>
#include <vector>

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

ReaderEnumeration enumerate_smart_card_readers() {
    ReaderEnumeration result;
    SCARDCONTEXT context = 0;
    LONG status = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &context);
    if (status != SCARD_S_SUCCESS) {
        result.error_code = status;
        return result;
    }

    DWORD characters = 0;
    status = SCardListReadersW(context, nullptr, nullptr, &characters);
    if (status == SCARD_E_NO_READERS_AVAILABLE) {
        result.success = true;
        SCardReleaseContext(context);
        return result;
    }
    if (status != SCARD_S_SUCCESS) {
        result.error_code = status;
        SCardReleaseContext(context);
        return result;
    }

    std::vector<wchar_t> names(characters, L'\0');
    status = SCardListReadersW(context, nullptr, names.data(), &characters);
    if (status != SCARD_S_SUCCESS) {
        result.error_code = status;
        SCardReleaseContext(context);
        return result;
    }

    const wchar_t* cursor = names.data();
    while (*cursor != L'\0') {
        SmartCardReader reader;
        reader.name = cursor;
        SCARD_READERSTATEW state{};
        state.szReader = reader.name.c_str();
        state.dwCurrentState = SCARD_STATE_UNAWARE;
        status = SCardGetStatusChangeW(context, 0, &state, 1);
        if (status == SCARD_S_SUCCESS) {
            reader.state = state.dwEventState;
            reader.card_present = (state.dwEventState & SCARD_STATE_PRESENT) != 0;
            reader.unavailable = (state.dwEventState &
                (SCARD_STATE_UNAVAILABLE | SCARD_STATE_UNKNOWN | SCARD_STATE_IGNORE)) != 0;
        } else {
            reader.unavailable = true;
        }
        result.readers.push_back(std::move(reader));
        cursor += std::wcslen(cursor) + 1U;
    }
    result.success = true;
    SCardReleaseContext(context);
    return result;
}

A3State diagnose_a3_state(const A3Evidence& evidence) noexcept {
    if (evidence.service != ServiceState::running) return A3State::service_unavailable;
    if (!evidence.reader_detected) return A3State::reader_missing;
    if (!evidence.device_present) return A3State::device_absent;
    if (!evidence.provider_detected) return A3State::middleware_missing;
    if (!evidence.key_associated) return A3State::key_unavailable;
    return A3State::apparently_ready;
}

}  // namespace certradar

#include "certradar/actions.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cryptuiapi.h>

namespace certradar {

const std::vector<AllowedAction>& allowed_actions() {
    static const std::vector<AllowedAction> actions{
        {"smartcard_service_start", "Iniciar o servico de Cartao Inteligente e repetir a deteccao.", true, true},
        {"certificate_import_wizard", "Abrir o assistente oficial de importacao do Windows.", false, false},
        {"official_middleware_page", "Abrir a pagina oficial cadastrada para o middleware identificado.", false, false},
    };
    return actions;
}

const AllowedAction* find_allowed_action(const std::string& id) noexcept {
    for (const auto& action : allowed_actions()) {
        if (action.id == id) return &action;
    }
    return nullptr;
}

ActionAuthorization authorize_action(
    const std::string& id,
    const bool explicit_consent) noexcept {
    if (find_allowed_action(id) == nullptr) return ActionAuthorization::unknown_action;
    return explicit_consent ? ActionAuthorization::allowed : ActionAuthorization::consent_required;
}

ServiceActionPlan plan_smartcard_service_start(
    const ServiceState current_state,
    const bool explicit_consent) noexcept {
    if (!explicit_consent) return ServiceActionPlan::blocked;
    if (current_state == ServiceState::running) return ServiceActionPlan::no_change;
    return ServiceActionPlan::start;
}

ServiceActionResult execute_smartcard_service_start(const bool explicit_consent) {
    ServiceActionResult result;
    result.before = query_smart_card_service().state;
    result.after = result.before;
    const auto plan = plan_smartcard_service_start(result.before, explicit_consent);
    if (plan == ServiceActionPlan::blocked) return result;
    if (plan == ServiceActionPlan::no_change) {
        result.state = ActionExecutionState::no_change;
        return result;
    }

    SC_HANDLE manager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
    if (manager == nullptr) {
        result.state = ActionExecutionState::failed;
        result.error_code = GetLastError();
        return result;
    }
    SC_HANDLE service = OpenServiceW(manager, L"SCardSvr", SERVICE_START | SERVICE_QUERY_STATUS);
    if (service == nullptr) {
        result.state = ActionExecutionState::failed;
        result.error_code = GetLastError();
        CloseServiceHandle(manager);
        return result;
    }

    const BOOL started = StartServiceW(service, 0, nullptr);
    const DWORD start_error = started == FALSE ? GetLastError() : ERROR_SUCCESS;
    if (started == FALSE && start_error != ERROR_SERVICE_ALREADY_RUNNING) {
        result.state = ActionExecutionState::failed;
        result.error_code = start_error;
    } else {
        for (int attempt = 0; attempt < 20; ++attempt) {
            result.after = query_smart_card_service().state;
            if (result.after == ServiceState::running) break;
            Sleep(100);
        }
        result.state = result.after == ServiceState::running
            ? ActionExecutionState::applied : ActionExecutionState::failed;
        if (result.state == ActionExecutionState::failed) result.error_code = ERROR_SERVICE_REQUEST_TIMEOUT;
    }
    CloseServiceHandle(service);
    CloseServiceHandle(manager);
    return result;
}

ImportActionPlan plan_certificate_import(
    const CandidateState candidate_state,
    const bool explicit_consent) noexcept {
    if (!explicit_consent || candidate_state != CandidateState::recognized) {
        return ImportActionPlan::blocked;
    }
    return ImportActionPlan::open_wizard;
}

ImportActionResult open_certificate_import_wizard(
    const std::filesystem::path& candidate,
    const bool explicit_consent,
    void* const parent_window) {
    ImportActionResult result;
    const auto candidate_state = inspect_pkcs12_container(candidate, 32ULL * 1024ULL * 1024ULL);
    if (plan_certificate_import(candidate_state, explicit_consent) != ImportActionPlan::open_wizard) {
        return result;
    }

    CRYPTUI_WIZ_IMPORT_SRC_INFO source{};
    source.dwSize = sizeof(source);
    source.dwSubjectChoice = CRYPTUI_WIZ_IMPORT_SUBJECT_FILE;
    source.pwszFileName = candidate.c_str();
    source.dwFlags = 0;
    source.pwszPassword = nullptr;
    const BOOL imported = CryptUIWizImport(
        0, reinterpret_cast<HWND>(parent_window), L"Importar certificado com o Windows",
        &source, nullptr);
    if (imported == FALSE) {
        result.state = ActionExecutionState::failed;
        result.error_code = GetLastError();
        return result;
    }
    result.state = ActionExecutionState::applied;
    return result;
}

}  // namespace certradar

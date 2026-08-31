#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>

#include "certradar/certificates.hpp"
#include "certradar/search.hpp"
#include "certradar/search_plan.hpp"
#include "certradar/platform.hpp"
#include "certradar/ui_model.hpp"

#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr int start_button_id = 1001;
constexpr int pause_button_id = 1002;
constexpr int cancel_button_id = 1003;
constexpr int results_list_id = 1004;
constexpr int status_label_id = 1005;
constexpr int copy_summary_button_id = 1006;
constexpr int reveal_candidate_button_id = 1007;
constexpr int environment_label_id = 1008;
constexpr int installed_certificates_button_id = 1009;
constexpr int machine_certificates_button_id = 1010;
constexpr UINT scan_progress_message = WM_APP + 1;
constexpr UINT scan_finished_message = WM_APP + 2;
constexpr UINT certificates_finished_message = WM_APP + 3;

HWND main_window = nullptr;
HWND start_button = nullptr;
HWND pause_button = nullptr;
HWND cancel_button = nullptr;
HWND results_list = nullptr;
HWND status_label = nullptr;
HWND copy_summary_button = nullptr;
HWND reveal_candidate_button = nullptr;
HWND environment_label = nullptr;
HWND installed_certificates_button = nullptr;
HWND machine_certificates_button = nullptr;
std::thread scan_thread;
std::thread certificate_thread;
std::unique_ptr<certradar::SearchControl> scan_control;
std::mutex result_mutex;
certradar::SearchResult completed_result;
std::mutex certificate_mutex;
certradar::CertificateStoreResult completed_certificates;
bool certificate_enumeration_failed = false;
bool shell_actions_available = false;
std::optional<certradar::WindowsPlatform> detected_platform;

enum class ResultView : std::uint8_t { search_candidates, installed_certificates };
ResultView result_view = ResultView::search_candidates;

class ComApartment final {
public:
    ComApartment() noexcept
        : status_(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)) {}

    ~ComApartment() {
        if (SUCCEEDED(status_)) CoUninitialize();
    }

    bool available() const noexcept {
        return SUCCEEDED(status_) || status_ == RPC_E_CHANGED_MODE;
    }

private:
    HRESULT status_;
};

HMENU control_identifier(const int identifier) noexcept {
    return reinterpret_cast<HMENU>(static_cast<INT_PTR>(identifier));
}

void set_status(const std::wstring& text) {
    SetWindowTextW(status_label, text.c_str());
}

void set_scan_controls(const bool running) {
    EnableWindow(start_button, running ? FALSE : TRUE);
    EnableWindow(pause_button, running ? TRUE : FALSE);
    EnableWindow(cancel_button, running ? TRUE : FALSE);
    EnableWindow(installed_certificates_button, running ? FALSE : TRUE);
    EnableWindow(machine_certificates_button, running ? FALSE : TRUE);
}

std::wstring store_owner(const certradar::StoreScope scope) {
    return scope == certradar::StoreScope::current_user ? L"do usuário" : L"da máquina";
}

void finish_previous_thread() {
    if (scan_thread.joinable()) scan_thread.join();
}

void finish_certificate_thread() {
    if (certificate_thread.joinable()) certificate_thread.join();
}

void start_scan() {
    finish_previous_thread();
    finish_certificate_thread();
    result_view = ResultView::search_candidates;
    SendMessageW(results_list, LB_RESETCONTENT, 0, 0);
    set_status(L"Preparando busca local...");
    set_scan_controls(true);
    EnableWindow(copy_summary_button, FALSE);
    EnableWindow(reveal_candidate_button, FALSE);
    SetWindowTextW(pause_button, L"Pausar");
    scan_control = std::make_unique<certradar::SearchControl>();
    auto* const control = scan_control.get();

    scan_thread = std::thread([control] {
        certradar::SearchResult result;
        try {
            const auto plan = certradar::build_default_search_plan();
            std::vector<std::filesystem::path> roots;
            roots.reserve(plan.size());
            for (const auto& item : plan) roots.push_back(item.path);

            certradar::SearchOptions options;
            options.recursive = true;
            options.validate_containers = true;
            result = certradar::search_files(
                roots, options, *control,
                [](const certradar::SearchProgress& progress) {
                    PostMessageW(
                        main_window, scan_progress_message,
                        static_cast<WPARAM>(progress.directories_visited),
                        static_cast<LPARAM>(progress.candidates));
                });
        } catch (...) {
            result.status = certradar::ScanStatus::failed;
            ++result.errors;
        }
        {
            const std::lock_guard<std::mutex> lock(result_mutex);
            completed_result = std::move(result);
        }
        PostMessageW(main_window, scan_finished_message, 0, 0);
    });
}

void show_installed_certificates(const certradar::StoreScope scope) {
    finish_previous_thread();
    finish_certificate_thread();
    result_view = ResultView::installed_certificates;
    SendMessageW(results_list, LB_RESETCONTENT, 0, 0);
    EnableWindow(copy_summary_button, FALSE);
    EnableWindow(reveal_candidate_button, FALSE);
    EnableWindow(start_button, FALSE);
    EnableWindow(installed_certificates_button, FALSE);
    EnableWindow(machine_certificates_button, FALSE);
    set_status(L"Lendo certificados pessoais " + store_owner(scope) +
               L" em modo somente leitura...");

    certificate_thread = std::thread([scope] {
        certradar::CertificateStoreResult result;
        result.scope = scope;
        bool failed = false;
        try {
            result = certradar::enumerate_personal_certificates(scope);
        } catch (...) {
            failed = true;
        }
        {
            const std::lock_guard<std::mutex> lock(certificate_mutex);
            completed_certificates = std::move(result);
            certificate_enumeration_failed = failed;
        }
        PostMessageW(main_window, certificates_finished_message, 0, 0);
    });
}

void show_installed_certificate_result() {
    finish_certificate_thread();
    certradar::CertificateStoreResult result;
    bool failed = false;
    {
        const std::lock_guard<std::mutex> lock(certificate_mutex);
        result = completed_certificates;
        failed = certificate_enumeration_failed;
    }
    if (failed) {
        set_status(L"A leitura do store Pessoal " + store_owner(result.scope) +
                   L" falhou sem alterar certificados.");
        set_scan_controls(false);
        return;
    }

    if (!result.opened) {
        set_status(L"Não foi possível abrir o store Pessoal " + store_owner(result.scope) +
                   L". Código: " +
                   std::to_wstring(result.error_code) + L".");
        set_scan_controls(false);
        return;
    }

    if (result.certificates.empty()) {
        const std::wstring empty = L"Nenhum certificado foi encontrado no store Pessoal " +
                                   store_owner(result.scope) + L".";
        SendMessageW(results_list, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(empty.c_str()));
    } else {
        const auto display_order = certradar::build_certificate_display_order(result);
        for (std::size_t row = 0; row < display_order.size(); ++row) {
            const auto label = certradar::format_certificate_summary(
                result.certificates[display_order[row]], row + 1);
            SendMessageW(results_list, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
        }
    }
    set_status(std::to_wstring(result.certificates.size()) +
               L" certificado(s) no store Pessoal " + store_owner(result.scope) +
               L". Situações que exigem atenção aparecem primeiro.");
    set_scan_controls(false);
    EnableWindow(copy_summary_button, TRUE);
}

bool reveal_in_explorer(const std::filesystem::path& path) {
    if (!shell_actions_available) return false;
    PIDLIST_ABSOLUTE item = ILCreateFromPathW(path.c_str());
    if (item == nullptr) return false;
    const HRESULT result = SHOpenFolderAndSelectItems(item, 0, nullptr, 0);
    ILFree(item);
    return SUCCEEDED(result);
}

void reveal_selected_candidate() {
    const LRESULT selection = SendMessageW(results_list, LB_GETCURSEL, 0, 0);
    if (selection == LB_ERR) {
        set_status(L"Selecione um candidato antes de mostrar o arquivo.");
        return;
    }

    certradar::SearchResult result;
    {
        const std::lock_guard<std::mutex> lock(result_mutex);
        result = completed_result;
    }
    const auto plan = certradar::build_candidate_reveal_plan(
        result, static_cast<std::size_t>(selection));
    if (plan.status != certradar::CandidateRevealStatus::ready) {
        set_status(L"O candidato selecionado não possui um caminho local seguro.");
        return;
    }
    if (reveal_in_explorer(plan.path)) {
        set_status(L"O Explorer foi aberto com o arquivo selecionado. O arquivo não foi executado.");
    } else {
        set_status(L"Não foi possível mostrar o arquivo. Ele pode ter sido movido ou removido.");
    }
}

bool copy_unicode_text_to_clipboard(const std::wstring& text) {
    const auto bytes = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (memory == nullptr) return false;

    void* const destination = GlobalLock(memory);
    if (destination == nullptr) {
        GlobalFree(memory);
        return false;
    }
    std::memcpy(destination, text.c_str(), bytes);
    GlobalUnlock(memory);

    if (OpenClipboard(main_window) == FALSE) {
        GlobalFree(memory);
        return false;
    }
    if (EmptyClipboard() == FALSE) {
        GlobalFree(memory);
        CloseClipboard();
        return false;
    }
    if (SetClipboardData(CF_UNICODETEXT, memory) == nullptr) {
        GlobalFree(memory);
        CloseClipboard();
        return false;
    }
    CloseClipboard();
    return true;
}

void copy_support_summary() {
    std::wstring summary;
    if (result_view == ResultView::installed_certificates) {
        certradar::CertificateStoreResult result;
        {
            const std::lock_guard<std::mutex> lock(certificate_mutex);
            result = completed_certificates;
        }
        summary = detected_platform.has_value()
            ? certradar::build_certificate_store_support_summary(result, *detected_platform)
            : certradar::build_certificate_store_support_summary(result);
    } else {
        certradar::SearchResult result;
        {
            const std::lock_guard<std::mutex> lock(result_mutex);
            result = completed_result;
        }
        summary = detected_platform.has_value()
            ? certradar::build_search_support_summary(result, *detected_platform)
            : certradar::build_search_support_summary(result);
    }
    if (copy_unicode_text_to_clipboard(summary)) {
        set_status(L"Resumo sanitizado copiado. Ele não contém nomes nem identificadores completos.");
    } else {
        set_status(L"Não foi possível acessar a área de transferência. Tente novamente.");
    }
}

void pause_or_resume() {
    if (!scan_control) return;
    if (scan_control->is_paused()) {
        scan_control->resume();
        SetWindowTextW(pause_button, L"Pausar");
        set_status(L"Busca retomada...");
    } else {
        scan_control->request_pause();
        SetWindowTextW(pause_button, L"Continuar");
        set_status(L"Pausa solicitada...");
    }
}

void show_completed_result() {
    finish_previous_thread();
    result_view = ResultView::search_candidates;
    certradar::SearchResult result;
    {
        const std::lock_guard<std::mutex> lock(result_mutex);
        result = completed_result;
    }
    for (const auto& candidate : result.candidates) {
        const auto label = certradar::format_candidate_label(candidate);
        SendMessageW(results_list, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
    }
    const std::wstring status =
        (result.status == certradar::ScanStatus::cancelled ? L"Busca cancelada. " : L"Busca concluída. ") +
        std::to_wstring(result.candidates.size()) + L" candidato(s), " +
        std::to_wstring(result.access_denied_count) + L" pasta(s) sem acesso. Nenhum arquivo foi alterado.";
    set_status(status);
    set_scan_controls(false);
    EnableWindow(copy_summary_button, TRUE);
}

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
        case WM_CREATE:
            {
                std::wstring environment_text = L"Ambiente: não foi possível detectar o Windows.";
                try {
                    detected_platform = certradar::detect_windows_platform();
                    environment_text = certradar::format_platform_summary(*detected_platform);
                } catch (...) {
                    detected_platform.reset();
                    environment_text = L"Ambiente: não foi possível detectar o Windows.";
                }
                environment_label = CreateWindowW(
                    L"STATIC", environment_text.c_str(), WS_CHILD | WS_VISIBLE,
                    16, 58, 740, 22, window,
                    control_identifier(environment_label_id), nullptr, nullptr);
            }
            start_button = CreateWindowW(L"BUTTON", L"Iniciar busca", WS_CHILD | WS_VISIBLE,
                16, 16, 110, 32, window, control_identifier(start_button_id), nullptr, nullptr);
            pause_button = CreateWindowW(L"BUTTON", L"Pausar", WS_CHILD | WS_VISIBLE | WS_DISABLED,
                134, 16, 80, 32, window, control_identifier(pause_button_id), nullptr, nullptr);
            cancel_button = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE | WS_DISABLED,
                222, 16, 80, 32, window, control_identifier(cancel_button_id), nullptr, nullptr);
            copy_summary_button = CreateWindowW(
                L"BUTTON", L"Copiar resumo", WS_CHILD | WS_VISIBLE | WS_DISABLED,
                310, 16, 120, 32, window, control_identifier(copy_summary_button_id), nullptr, nullptr);
            reveal_candidate_button = CreateWindowW(
                L"BUTTON", L"Mostrar arquivo", WS_CHILD | WS_VISIBLE | WS_DISABLED,
                438, 16, 125, 32, window,
                control_identifier(reveal_candidate_button_id), nullptr, nullptr);
            installed_certificates_button = CreateWindowW(
                L"BUTTON", L"Cert. usuário", WS_CHILD | WS_VISIBLE,
                571, 16, 95, 32, window,
                control_identifier(installed_certificates_button_id), nullptr, nullptr);
            machine_certificates_button = CreateWindowW(
                L"BUTTON", L"Cert. máquina", WS_CHILD | WS_VISIBLE,
                674, 16, 102, 32, window,
                control_identifier(machine_certificates_button_id), nullptr, nullptr);
            status_label = CreateWindowW(L"STATIC", L"Pronto. A busca só começa com sua autorização.",
                WS_CHILD | WS_VISIBLE, 16, 84, 740, 42, window,
                control_identifier(status_label_id), nullptr, nullptr);
            results_list = CreateWindowW(L"LISTBOX", nullptr,
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOINTEGRALHEIGHT,
                16, 130, 740, 324, window,
                control_identifier(results_list_id), nullptr, nullptr);
            return 0;
        case WM_SIZE:
            MoveWindow(results_list, 16, 130, LOWORD(lparam) - 32, HIWORD(lparam) - 146, TRUE);
            MoveWindow(status_label, 16, 84, LOWORD(lparam) - 32, 42, TRUE);
            MoveWindow(environment_label, 16, 58, LOWORD(lparam) - 32, 22, TRUE);
            return 0;
        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case start_button_id: start_scan(); return 0;
                case pause_button_id: pause_or_resume(); return 0;
                case cancel_button_id:
                    if (scan_control) scan_control->cancel();
                    set_status(L"Cancelamento solicitado...");
                    return 0;
                case copy_summary_button_id: copy_support_summary(); return 0;
                case reveal_candidate_button_id: reveal_selected_candidate(); return 0;
                case installed_certificates_button_id:
                    show_installed_certificates(certradar::StoreScope::current_user);
                    return 0;
                case machine_certificates_button_id:
                    show_installed_certificates(certradar::StoreScope::local_machine);
                    return 0;
                case results_list_id:
                    if (HIWORD(wparam) == LBN_SELCHANGE) {
                        const auto selection = SendMessageW(results_list, LB_GETCURSEL, 0, 0);
                        const bool can_reveal =
                            result_view == ResultView::search_candidates && selection != LB_ERR;
                        EnableWindow(reveal_candidate_button, can_reveal ? TRUE : FALSE);
                    }
                    return 0;
                default:
                    break;
            }
            break;
        case scan_progress_message:
            set_status(L"Buscando: " + std::to_wstring(wparam) + L" pastas, " +
                       std::to_wstring(lparam) + L" candidato(s)...");
            return 0;
        case scan_finished_message:
            show_completed_result();
            return 0;
        case certificates_finished_message:
            show_installed_certificate_result();
            return 0;
        case WM_DESTROY:
            if (scan_control) scan_control->cancel();
            finish_previous_thread();
            finish_certificate_thread();
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    ComApartment apartment;
    shell_actions_available = apartment.available();

    INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&controls);

    const wchar_t class_name[] = L"CertRadarMainWindow";
    WNDCLASSW window_class{};
    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = instance;
    window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    window_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    window_class.lpszClassName = class_name;
    if (RegisterClassW(&window_class) == 0) return 1;

    main_window = CreateWindowExW(
        0, class_name, L"CertRadar — suporte a certificados digitais",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 540,
        nullptr, nullptr, instance, nullptr);
    if (main_window == nullptr) return 1;

    ShowWindow(main_window, show_command);
    UpdateWindow(main_window);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    return static_cast<int>(message.wParam);
}

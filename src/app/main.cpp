#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commctrl.h>

#include "certradar/search.hpp"
#include "certradar/search_plan.hpp"
#include "certradar/ui_model.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr int start_button_id = 1001;
constexpr int pause_button_id = 1002;
constexpr int cancel_button_id = 1003;
constexpr int results_list_id = 1004;
constexpr int status_label_id = 1005;
constexpr UINT scan_progress_message = WM_APP + 1;
constexpr UINT scan_finished_message = WM_APP + 2;

HWND main_window = nullptr;
HWND start_button = nullptr;
HWND pause_button = nullptr;
HWND cancel_button = nullptr;
HWND results_list = nullptr;
HWND status_label = nullptr;
std::thread scan_thread;
std::unique_ptr<certradar::SearchControl> scan_control;
std::mutex result_mutex;
certradar::SearchResult completed_result;

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
}

void finish_previous_thread() {
    if (scan_thread.joinable()) scan_thread.join();
}

void start_scan() {
    finish_previous_thread();
    SendMessageW(results_list, LB_RESETCONTENT, 0, 0);
    set_status(L"Preparando busca local...");
    set_scan_controls(true);
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
}

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
        case WM_CREATE:
            start_button = CreateWindowW(L"BUTTON", L"Iniciar busca", WS_CHILD | WS_VISIBLE,
                16, 16, 130, 32, window, control_identifier(start_button_id), nullptr, nullptr);
            pause_button = CreateWindowW(L"BUTTON", L"Pausar", WS_CHILD | WS_VISIBLE | WS_DISABLED,
                154, 16, 100, 32, window, control_identifier(pause_button_id), nullptr, nullptr);
            cancel_button = CreateWindowW(L"BUTTON", L"Cancelar", WS_CHILD | WS_VISIBLE | WS_DISABLED,
                262, 16, 100, 32, window, control_identifier(cancel_button_id), nullptr, nullptr);
            status_label = CreateWindowW(L"STATIC", L"Pronto. A busca só começa com sua autorização.",
                WS_CHILD | WS_VISIBLE, 16, 58, 740, 42, window,
                control_identifier(status_label_id), nullptr, nullptr);
            results_list = CreateWindowW(L"LISTBOX", nullptr,
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOINTEGRALHEIGHT,
                16, 104, 740, 350, window,
                control_identifier(results_list_id), nullptr, nullptr);
            return 0;
        case WM_SIZE:
            MoveWindow(results_list, 16, 104, LOWORD(lparam) - 32, HIWORD(lparam) - 120, TRUE);
            MoveWindow(status_label, 16, 58, LOWORD(lparam) - 32, 42, TRUE);
            return 0;
        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case start_button_id: start_scan(); return 0;
                case pause_button_id: pause_or_resume(); return 0;
                case cancel_button_id:
                    if (scan_control) scan_control->cancel();
                    set_status(L"Cancelamento solicitado...");
                    return 0;
            }
            break;
        case scan_progress_message:
            set_status(L"Buscando: " + std::to_wstring(wparam) + L" pastas, " +
                       std::to_wstring(lparam) + L" candidato(s)...");
            return 0;
        case scan_finished_message:
            show_completed_result();
            return 0;
        case WM_DESTROY:
            if (scan_control) scan_control->cancel();
            finish_previous_thread();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
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
        0, class_name, L"CertRadar — Busca segura de certificados A1",
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

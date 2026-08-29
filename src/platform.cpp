#include "certradar/platform.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdexcept>

namespace certradar {
namespace {

using RtlGetVersionFunction = LONG(WINAPI*)(OSVERSIONINFOEXW*);

std::string utf8_from_wide(const wchar_t* text) {
    if (text == nullptr || *text == L'\0') return {};
    const int required = WideCharToMultiByte(CP_UTF8, 0, text, -1, nullptr, 0, nullptr, nullptr);
    if (required <= 1) return {};
    std::string value(static_cast<std::size_t>(required), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text, -1, value.data(), required, nullptr, nullptr);
    value.pop_back();
    return value;
}

std::string architecture_name(const WORD architecture) {
    switch (architecture) {
        case PROCESSOR_ARCHITECTURE_INTEL: return "x86";
        case PROCESSOR_ARCHITECTURE_AMD64: return "x64";
        case PROCESSOR_ARCHITECTURE_ARM64: return "arm64";
        default: return "unknown";
    }
}

bool process_is_elevated() noexcept {
    HANDLE token = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token) == FALSE) return false;
    TOKEN_ELEVATION elevation{};
    DWORD received = 0;
    const bool result = GetTokenInformation(
        token, TokenElevation, &elevation, sizeof(elevation), &received) != FALSE &&
        elevation.TokenIsElevated != 0;
    CloseHandle(token);
    return result;
}

}  // namespace

WindowsPlatform detect_windows_platform() {
    const HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (ntdll == nullptr) throw std::runtime_error("ntdll.dll is unavailable");

    const auto rtl_get_version =
        reinterpret_cast<RtlGetVersionFunction>(GetProcAddress(ntdll, "RtlGetVersion"));
    if (rtl_get_version == nullptr) throw std::runtime_error("RtlGetVersion is unavailable");

    OSVERSIONINFOEXW version{};
    version.dwOSVersionInfoSize = sizeof(version);
    if (rtl_get_version(&version) != 0) throw std::runtime_error("Windows version query failed");

    SYSTEM_INFO system_info{};
    GetNativeSystemInfo(&system_info);

    DWORD product_type = 0;
    const bool has_product = GetProductInfo(
        version.dwMajorVersion,
        version.dwMinorVersion,
        version.wServicePackMajor,
        version.wServicePackMinor,
        &product_type) != FALSE;

    WindowsPlatform result;
    result.major = version.dwMajorVersion;
    result.minor = version.dwMinorVersion;
    result.build = version.dwBuildNumber;
    result.service_pack_major = version.wServicePackMajor;
    result.service_pack = utf8_from_wide(version.szCSDVersion);
    result.architecture = architecture_name(system_info.wProcessorArchitecture);
    result.edition = has_product ? "product-" + std::to_string(product_type) : "unknown";
    result.elevated = process_is_elevated();
    return result;
}

PlatformFacts to_platform_facts(const WindowsPlatform& platform) {
    PlatformFacts facts;
    facts.version_major = platform.major;
    facts.version_minor = platform.minor;
    facts.build = platform.build;
    facts.architecture = platform.architecture;
    facts.edition = platform.edition;
    facts.service_pack = platform.service_pack;
    facts.elevated = platform.elevated;
    return facts;
}

}  // namespace certradar

#pragma once

#include "certradar/facts.hpp"

#include <cstdint>
#include <string>

namespace certradar {

struct WindowsPlatform {
    std::uint32_t major{0};
    std::uint32_t minor{0};
    std::uint32_t build{0};
    std::uint16_t service_pack_major{0};
    std::string service_pack;
    std::string architecture{"unknown"};
    std::string edition;
    bool elevated{false};
};

WindowsPlatform detect_windows_platform();
SupportMode classify_support_mode(const WindowsPlatform& platform) noexcept;
PlatformFacts to_platform_facts(const WindowsPlatform& platform);

}  // namespace certradar

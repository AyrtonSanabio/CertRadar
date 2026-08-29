#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace certradar {

enum class SupportMode { full, compatible, legacy, unsupported };
enum class ScanStatus { not_started, running, paused, completed, partial, cancelled, failed };

struct PlatformFacts {
    std::string family{"windows"};
    std::uint32_t version_major{0};
    std::uint32_t version_minor{0};
    std::uint32_t build{0};
    std::string architecture{"unknown"};
    std::string edition;
    std::string service_pack;
    bool elevated{false};
    SupportMode mode{SupportMode::unsupported};
};

struct ScanFacts {
    ScanStatus status{ScanStatus::not_started};
    std::string phase{"not_started"};
    std::uint64_t directories_visited{0};
    std::uint64_t access_denied_count{0};
    std::uint64_t candidates{0};
};

struct ErrorFact {
    std::string code;
    std::string category;
    std::string message;
};

struct FactDocument {
    static constexpr std::uint32_t current_schema_version = 1;

    std::uint32_t schema_version{current_schema_version};
    PlatformFacts platform;
    ScanFacts scan;
    std::vector<ErrorFact> errors;
};

std::string support_mode_name(SupportMode mode);
std::string scan_status_name(ScanStatus status);
std::string serialize_facts(const FactDocument& document);
FactDocument deserialize_facts(const std::string& json_text);

}  // namespace certradar

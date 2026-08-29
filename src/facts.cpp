#include "certradar/facts.hpp"

#include <nlohmann/json.hpp>

#include <stdexcept>

namespace certradar {
namespace {

SupportMode parse_support_mode(const std::string& value) {
    if (value == "full") return SupportMode::full;
    if (value == "compatible") return SupportMode::compatible;
    if (value == "legacy") return SupportMode::legacy;
    return SupportMode::unsupported;
}

ScanStatus parse_scan_status(const std::string& value) {
    if (value == "running") return ScanStatus::running;
    if (value == "paused") return ScanStatus::paused;
    if (value == "completed") return ScanStatus::completed;
    if (value == "partial") return ScanStatus::partial;
    if (value == "cancelled") return ScanStatus::cancelled;
    if (value == "failed") return ScanStatus::failed;
    return ScanStatus::not_started;
}

}  // namespace

std::string support_mode_name(const SupportMode mode) {
    switch (mode) {
        case SupportMode::full: return "full";
        case SupportMode::compatible: return "compatible";
        case SupportMode::legacy: return "legacy";
        case SupportMode::unsupported: return "unsupported";
    }
    return "unsupported";
}

std::string scan_status_name(const ScanStatus status) {
    switch (status) {
        case ScanStatus::not_started: return "not_started";
        case ScanStatus::running: return "running";
        case ScanStatus::paused: return "paused";
        case ScanStatus::completed: return "completed";
        case ScanStatus::partial: return "partial";
        case ScanStatus::cancelled: return "cancelled";
        case ScanStatus::failed: return "failed";
    }
    return "failed";
}

std::string serialize_facts(const FactDocument& document) {
    nlohmann::json errors = nlohmann::json::array();
    for (const auto& error : document.errors) {
        errors.push_back({
            {"code", error.code},
            {"category", error.category},
            {"message", error.message},
        });
    }

    const nlohmann::json output = {
        {"schemaVersion", document.schema_version},
        {"platform", {
            {"family", document.platform.family},
            {"versionMajor", document.platform.version_major},
            {"versionMinor", document.platform.version_minor},
            {"build", document.platform.build},
            {"architecture", document.platform.architecture},
            {"edition", document.platform.edition},
            {"servicePack", document.platform.service_pack},
            {"elevated", document.platform.elevated},
            {"mode", support_mode_name(document.platform.mode)},
        }},
        {"scan", {
            {"status", scan_status_name(document.scan.status)},
            {"phase", document.scan.phase},
            {"directoriesVisited", document.scan.directories_visited},
            {"accessDeniedCount", document.scan.access_denied_count},
            {"candidates", document.scan.candidates},
        }},
        {"certificates", nlohmann::json::array()},
        {"smartCards", nlohmann::json::array()},
        {"errors", errors},
    };
    return output.dump(2);
}

FactDocument deserialize_facts(const std::string& json_text) {
    const auto input = nlohmann::json::parse(json_text);
    const auto version = input.at("schemaVersion").get<std::uint32_t>();
    if (version != FactDocument::current_schema_version) {
        throw std::invalid_argument("Unsupported facts schema version: " + std::to_string(version));
    }

    FactDocument document;
    document.schema_version = version;
    const auto& platform = input.at("platform");
    document.platform.family = platform.at("family").get<std::string>();
    document.platform.version_major = platform.at("versionMajor").get<std::uint32_t>();
    document.platform.version_minor = platform.at("versionMinor").get<std::uint32_t>();
    document.platform.build = platform.at("build").get<std::uint32_t>();
    document.platform.architecture = platform.at("architecture").get<std::string>();
    document.platform.edition = platform.at("edition").get<std::string>();
    document.platform.service_pack = platform.at("servicePack").get<std::string>();
    document.platform.elevated = platform.at("elevated").get<bool>();
    document.platform.mode = parse_support_mode(platform.at("mode").get<std::string>());

    const auto& scan = input.at("scan");
    document.scan.status = parse_scan_status(scan.at("status").get<std::string>());
    document.scan.phase = scan.at("phase").get<std::string>();
    document.scan.directories_visited = scan.at("directoriesVisited").get<std::uint64_t>();
    document.scan.access_denied_count = scan.at("accessDeniedCount").get<std::uint64_t>();
    document.scan.candidates = scan.at("candidates").get<std::uint64_t>();

    for (const auto& error : input.at("errors")) {
        document.errors.push_back({
            error.at("code").get<std::string>(),
            error.at("category").get<std::string>(),
            error.at("message").get<std::string>(),
        });
    }
    return document;
}

}  // namespace certradar

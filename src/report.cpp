#include "certradar/report.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>

namespace certradar {

std::string mask_identifier(const std::string& value) {
    std::string digits;
    for (const char character : value) {
        if (std::isdigit(static_cast<unsigned char>(character)) != 0) digits.push_back(character);
    }
    if (digits.size() < 4) return "***";
    return "***" + digits.substr(digits.size() - 4);
}

std::string mask_path(const std::string& path) {
    const auto separator = path.find_last_of("/\\");
    return separator == std::string::npos ? "[arquivo local]" : "[...]\\" + path.substr(separator + 1);
}

std::string build_support_summary(const std::vector<Finding>& findings) {
    std::string output = "Resumo CertRadar\n";
    if (findings.empty()) return output + "Nenhum problema foi identificado com os fatos coletados.\n";
    for (const auto& finding : findings) {
        output += "- " + finding.fact + " Proxima acao: " + finding.recommended_action + "\n";
    }
    return output;
}

std::string build_redacted_report_json(const std::vector<Finding>& findings) {
    nlohmann::json items = nlohmann::json::array();
    for (const auto& finding : findings) {
        items.push_back({
            {"id", finding.id}, {"fact", finding.fact},
            {"probableCause", finding.probable_cause},
            {"confidence", finding.confidence == Confidence::high ? "high" :
                finding.confidence == Confidence::medium ? "medium" : "low"},
            {"recommendedAction", finding.recommended_action}});
    }
    return nlohmann::json({
        {"reportVersion", 1}, {"localOnly", true},
        {"containsPrivateKey", false}, {"findings", items}}).dump(2);
}

}  // namespace certradar

#include "certradar/report.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>

namespace certradar {
namespace {

std::string lowercase_ascii(std::string value) {
    for (auto& character : value) {
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    }
    return value;
}

bool is_secret_keyword(const std::string& token) {
    auto normalized = lowercase_ascii(token);
    while (!normalized.empty() &&
           (normalized.back() == ':' || normalized.back() == '=')) {
        normalized.pop_back();
    }
    return normalized == "pin" || normalized == "senha" || normalized == "password";
}

bool has_inline_secret(const std::string& token) {
    const auto normalized = lowercase_ascii(token);
    for (const auto* const keyword : {"pin", "senha", "password"}) {
        const std::string prefix(keyword);
        if (normalized.size() > prefix.size() &&
            normalized.compare(0, prefix.size(), prefix) == 0 &&
            (normalized[prefix.size()] == ':' || normalized[prefix.size()] == '=')) {
            return true;
        }
    }
    return false;
}

bool contains_absolute_windows_path(const std::string& token) {
    for (std::size_t index = 0; index + 2 < token.size(); ++index) {
        const bool boundary = index == 0 || token[index - 1] == '=' || token[index - 1] == '"' ||
                              token[index - 1] == '\'' || token[index - 1] == '(';
        if (boundary && std::isalpha(static_cast<unsigned char>(token[index])) != 0 &&
            token[index + 1] == ':' && (token[index + 2] == '\\' || token[index + 2] == '/')) {
            return true;
        }
        if (boundary && token[index] == '\\' && token[index + 1] == '\\') return true;
    }
    return false;
}

bool contains_identifier(const std::string& token) {
    std::size_t digits = 0;
    for (const auto character : token) {
        if (std::isdigit(static_cast<unsigned char>(character)) != 0) ++digits;
    }
    return digits >= 9;
}

std::string sanitize_report_text(const std::string& text) {
    std::string output;
    output.reserve(text.size());
    bool redact_next_token = false;
    std::size_t position = 0;
    while (position < text.size()) {
        if (std::isspace(static_cast<unsigned char>(text[position])) != 0) {
            output.push_back(text[position++]);
            continue;
        }
        const auto start = position;
        while (position < text.size() &&
               std::isspace(static_cast<unsigned char>(text[position])) == 0) {
            ++position;
        }
        const auto token = text.substr(start, position - start);
        if (redact_next_token) {
            output += "[redigido]";
            redact_next_token = false;
        } else if (has_inline_secret(token)) {
            output += "[segredo-redigido]";
        } else if (is_secret_keyword(token)) {
            output += token;
            redact_next_token = true;
        } else if (contains_absolute_windows_path(token)) {
            output += "[caminho-local]";
        } else if (contains_identifier(token)) {
            output += "[identificador]";
        } else {
            output += token;
        }
    }
    return output;
}

}  // namespace

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
        output += "- " + sanitize_report_text(finding.fact) + " Proxima acao: " +
                  sanitize_report_text(finding.recommended_action) + "\n";
    }
    return output;
}

std::string build_redacted_report_json(const std::vector<Finding>& findings) {
    nlohmann::json items = nlohmann::json::array();
    for (const auto& finding : findings) {
        items.push_back({
            {"id", sanitize_report_text(finding.id)},
            {"fact", sanitize_report_text(finding.fact)},
            {"probableCause", sanitize_report_text(finding.probable_cause)},
            {"confidence", finding.confidence == Confidence::high ? "high" :
                finding.confidence == Confidence::medium ? "medium" : "low"},
            {"recommendedAction", sanitize_report_text(finding.recommended_action)}});
    }
    return nlohmann::json({
        {"reportVersion", 1}, {"localOnly", true},
        {"containsPrivateKey", false}, {"findings", items}}).dump(2);
}

}  // namespace certradar

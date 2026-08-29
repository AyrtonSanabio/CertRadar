#include "certradar/report.hpp"

#include <doctest/doctest.h>
#include <nlohmann/json.hpp>

TEST_CASE("privacy helpers keep only minimal identifying fragments") {
    CHECK(certradar::mask_identifier("123.456.789-01") == "***8901");
    CHECK(certradar::mask_path("C:/Users/Maria/Documents/certificado.pfx") == "[...]\\certificado.pfx");
}

TEST_CASE("support report is local only and contains no private key field") {
    const std::vector<certradar::Finding> findings{{
        "test", "Fato sanitizado", "Causa provavel", certradar::Confidence::medium, "Acao segura"}};
    const auto parsed = nlohmann::json::parse(certradar::build_redacted_report_json(findings));
    CHECK(parsed.at("localOnly") == true);
    CHECK(parsed.at("containsPrivateKey") == false);
    CHECK(parsed.at("findings").size() == 1);
    CHECK(certradar::build_support_summary(findings).find("Proxima acao") != std::string::npos);
}

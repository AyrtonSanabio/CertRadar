#include "certradar/report.hpp"

#include <doctest/doctest.h>

TEST_CASE("support outputs redact identifiers paths PINs and passwords defensively") {
    const certradar::Finding hostile{
        "cliente-12345678900",
        "CPF 123.456.789-00 no arquivo C:\\Users\\Maria\\certificado.pfx",
        "PIN=9876 e senha segredo-total",
        certradar::Confidence::high,
        "Tentar password:nao-divulgar",
    };

    const auto json = certradar::build_redacted_report_json({hostile});
    const auto summary = certradar::build_support_summary({hostile});
    for (const auto& output : {json, summary}) {
        CHECK(output.find("123.456.789-00") == std::string::npos);
        CHECK(output.find("12345678900") == std::string::npos);
        CHECK(output.find("C:\\Users\\Maria") == std::string::npos);
        CHECK(output.find("9876") == std::string::npos);
        CHECK(output.find("segredo-total") == std::string::npos);
        CHECK(output.find("nao-divulgar") == std::string::npos);
    }
}

TEST_CASE("privacy sanitizer preserves ordinary diagnostic language") {
    const certradar::Finding ordinary{
        "smartcard_service_stopped",
        "O servico SCardSvr esta parado.",
        "O Windows nao consegue enumerar leitores.",
        certradar::Confidence::high,
        "Autorize o inicio do servico.",
    };

    const auto summary = certradar::build_support_summary({ordinary});
    CHECK(summary.find("SCardSvr") != std::string::npos);
    CHECK(summary.find("Autorize o inicio") != std::string::npos);
}

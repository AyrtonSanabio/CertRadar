#include "certradar/application_profile.hpp"

#include <doctest/doctest.h>

#include <algorithm>

namespace {

const certradar::ProfileCheck* find_check(
    const certradar::ProfileEvaluation& evaluation,
    const std::string& id) {
    const auto found = std::find_if(
        evaluation.checks.begin(), evaluation.checks.end(),
        [&](const certradar::ProfileCheck& check) { return check.id == id; });
    return found == evaluation.checks.end() ? nullptr : &*found;
}

certradar::PjeProfileInput ready_a1_input() {
    certradar::PjeProfileInput input;
    input.certificate = certradar::EvidenceState::present;
    input.certificate_validity = certradar::CertificateValidity::valid;
    input.private_key = certradar::EvidenceState::present;
    input.trusted_chain = certradar::EvidenceState::present;
    input.pjeoffice_installed = certradar::EvidenceState::present;
    input.pjeoffice_running = certradar::EvidenceState::present;
    return input;
}

}  // namespace

TEST_CASE("PJe is a stable selectable application profile") {
    const auto* profile = certradar::find_application_profile("pje");
    REQUIRE(profile != nullptr);
    CHECK(profile->display_name == "PJe");
    CHECK(profile->official_documentation_url ==
          "https://docs.pje.jus.br/servicos-negociais/pjeoffice-pro/");
    CHECK(certradar::find_application_profile("perfil_desconhecido") == nullptr);
}

TEST_CASE("PJe profile is ready only when every required local fact passes") {
    const auto evaluation = certradar::evaluate_pje_profile(ready_a1_input());
    CHECK(evaluation.profile_id == "pje");
    CHECK(evaluation.status == certradar::ProfileStatus::ready);
    REQUIRE_FALSE(evaluation.checks.empty());
    for (const auto& check : evaluation.checks) {
        CHECK(check.status == certradar::ProfileCheckStatus::passed);
    }
    CHECK(find_check(evaluation, "external_device") == nullptr);
}

TEST_CASE("PJe profile reports missing PJeOffice without blaming the certificate") {
    auto input = ready_a1_input();
    input.pjeoffice_installed = certradar::EvidenceState::absent;
    input.pjeoffice_running = certradar::EvidenceState::unknown;
    const auto evaluation = certradar::evaluate_pje_profile(input);

    CHECK(evaluation.status == certradar::ProfileStatus::blocked);
    const auto* pjeoffice = find_check(evaluation, "pjeoffice_installation");
    REQUIRE(pjeoffice != nullptr);
    CHECK(pjeoffice->status == certradar::ProfileCheckStatus::blocked);
    CHECK(pjeoffice->recommended_action.find("oficial") != std::string::npos);
    const auto* certificate = find_check(evaluation, "certificate_presence");
    REQUIRE(certificate != nullptr);
    CHECK(certificate->status == certradar::ProfileCheckStatus::passed);
}

TEST_CASE("PJe profile keeps A3 middleware failure separate from other checks") {
    auto input = ready_a1_input();
    input.uses_external_device = true;
    input.external_device_state = certradar::A3State::middleware_missing;
    const auto evaluation = certradar::evaluate_pje_profile(input);

    CHECK(evaluation.status == certradar::ProfileStatus::blocked);
    const auto* device = find_check(evaluation, "external_device");
    REQUIRE(device != nullptr);
    CHECK(device->status == certradar::ProfileCheckStatus::blocked);
    CHECK(device->recommended_action.find("middleware") != std::string::npos);
}

TEST_CASE("unknown PJe evidence remains indeterminate and never becomes success") {
    const auto evaluation = certradar::evaluate_pje_profile({});
    CHECK(evaluation.status == certradar::ProfileStatus::indeterminate);
    for (const auto& check : evaluation.checks) {
        CHECK(check.status != certradar::ProfileCheckStatus::passed);
    }
}

TEST_CASE("PJe certificate expiry is blocked and near expiry requests attention") {
    auto input = ready_a1_input();
    input.certificate_validity = certradar::CertificateValidity::expired;
    auto evaluation = certradar::evaluate_pje_profile(input);
    CHECK(evaluation.status == certradar::ProfileStatus::blocked);
    REQUIRE(find_check(evaluation, "certificate_validity") != nullptr);
    CHECK(find_check(evaluation, "certificate_validity")->status ==
          certradar::ProfileCheckStatus::blocked);

    input.certificate_validity = certradar::CertificateValidity::expiring_soon;
    evaluation = certradar::evaluate_pje_profile(input);
    CHECK(evaluation.status == certradar::ProfileStatus::attention);
    CHECK(find_check(evaluation, "certificate_validity")->status ==
          certradar::ProfileCheckStatus::warning);
}

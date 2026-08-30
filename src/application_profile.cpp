#include "certradar/application_profile.hpp"

namespace certradar {
namespace {

ProfileCheck evidence_check(
    const std::string& id,
    const EvidenceState evidence,
    const std::string& present_fact,
    const std::string& absent_fact,
    const std::string& action) {
    if (evidence == EvidenceState::present) {
        return {id, ProfileCheckStatus::passed, present_fact, {}};
    }
    if (evidence == EvidenceState::absent) {
        return {id, ProfileCheckStatus::blocked, absent_fact, action};
    }
    return {id, ProfileCheckStatus::unknown,
            "A verificacao local ainda nao produziu evidencia suficiente.",
            "Execute a verificacao indicada antes de concluir o diagnostico."};
}

ProfileCheck validity_check(
    const EvidenceState certificate,
    const CertificateValidity validity) {
    if (certificate != EvidenceState::present) {
        return {"certificate_validity", ProfileCheckStatus::unknown,
                "A validade nao pode ser avaliada sem um certificado presente.",
                "Localize ou disponibilize o certificado antes de verificar a validade."};
    }
    switch (validity) {
        case CertificateValidity::valid:
            return {"certificate_validity", ProfileCheckStatus::passed,
                    "O certificado esta dentro do periodo de validade.", {}};
        case CertificateValidity::expiring_soon:
            return {"certificate_validity", ProfileCheckStatus::warning,
                    "O certificado esta valido, mas vence em ate 30 dias.",
                    "Planeje a renovacao sem interromper o atendimento atual."};
        case CertificateValidity::not_yet_valid:
            return {"certificate_validity", ProfileCheckStatus::blocked,
                    "O certificado ainda nao entrou no periodo de validade.",
                    "Confirme data e hora do Windows e o inicio de validade do certificado."};
        case CertificateValidity::expired:
            return {"certificate_validity", ProfileCheckStatus::blocked,
                    "O certificado esta expirado.",
                    "Use um certificado valido ou inicie a renovacao com a autoridade emissora."};
    }
    return {"certificate_validity", ProfileCheckStatus::unknown,
            "A validade nao foi classificada.", "Repita a leitura do certificado."};
}

ProfileCheck external_device_check(const A3State state) {
    switch (state) {
        case A3State::apparently_ready:
            return {"external_device", ProfileCheckStatus::passed,
                    "O dispositivo A3 esta aparentemente disponivel para o PJeOffice.", {}};
        case A3State::service_unavailable:
            return {"external_device", ProfileCheckStatus::blocked,
                    "O servico de cartao inteligente nao esta disponivel.",
                    "Autorize o inicio do servico e repita a deteccao."};
        case A3State::reader_missing:
            return {"external_device", ProfileCheckStatus::blocked,
                    "Nenhum leitor compativel foi detectado.",
                    "Verifique a conexao USB e o driver oficial do dispositivo."};
        case A3State::device_absent:
            return {"external_device", ProfileCheckStatus::blocked,
                    "O leitor existe, mas o token ou cartao nao esta presente.",
                    "Reconecte o dispositivo e repita a deteccao."};
        case A3State::middleware_missing:
            return {"external_device", ProfileCheckStatus::blocked,
                    "O middleware do dispositivo nao foi identificado.",
                    "Abra a pagina oficial do middleware identificado e conclua a instalacao manualmente."};
        case A3State::key_unavailable:
            return {"external_device", ProfileCheckStatus::blocked,
                    "O certificado nao apresenta associacao observavel com a chave do dispositivo.",
                    "Confirme o dispositivo e o provider sem remover certificados existentes."};
    }
    return {"external_device", ProfileCheckStatus::unknown,
            "O estado do dispositivo A3 nao foi classificado.",
            "Repita a verificacao local do dispositivo."};
}

ProfileStatus aggregate_status(const std::vector<ProfileCheck>& checks) noexcept {
    bool has_warning = false;
    bool has_unknown = false;
    for (const auto& check : checks) {
        if (check.status == ProfileCheckStatus::blocked) return ProfileStatus::blocked;
        has_warning = has_warning || check.status == ProfileCheckStatus::warning;
        has_unknown = has_unknown || check.status == ProfileCheckStatus::unknown;
    }
    if (has_warning) return ProfileStatus::attention;
    if (has_unknown) return ProfileStatus::indeterminate;
    return ProfileStatus::ready;
}

}  // namespace

const std::vector<ApplicationProfile>& application_profiles() {
    static const std::vector<ApplicationProfile> profiles{{
        "pje", "PJe", "https://docs.pje.jus.br/servicos-negociais/pjeoffice-pro/"}};
    return profiles;
}

const ApplicationProfile* find_application_profile(const std::string& id) noexcept {
    for (const auto& profile : application_profiles()) {
        if (profile.id == id) return &profile;
    }
    return nullptr;
}

ProfileEvaluation evaluate_pje_profile(const PjeProfileInput& input) {
    ProfileEvaluation evaluation;
    evaluation.profile_id = "pje";
    evaluation.checks.push_back(evidence_check(
        "certificate_presence", input.certificate,
        "Um certificado foi localizado para o objetivo PJe.",
        "Nenhum certificado foi localizado para o objetivo PJe.",
        "Localize o certificado A1 ou conecte o dispositivo A3 antes de tentar o acesso."));
    evaluation.checks.push_back(validity_check(input.certificate, input.certificate_validity));
    evaluation.checks.push_back(evidence_check(
        "private_key_association", input.private_key,
        "O certificado apresenta associacao observavel com chave privada.",
        "O certificado esta visivel, mas sem associacao observavel com chave privada.",
        "Localize o PFX/P12 original ou confirme o dispositivo A3; nao exclua o certificado atual."));
    evaluation.checks.push_back(evidence_check(
        "trusted_chain", input.trusted_chain,
        "A cadeia local do certificado foi construida sem erro de confianca.",
        "A cadeia local do certificado apresenta problema de confianca.",
        "Revise a causa exata da cadeia antes de instalar qualquer certificado."));
    evaluation.checks.push_back(evidence_check(
        "pjeoffice_installation", input.pjeoffice_installed,
        "O PJeOffice foi identificado na maquina.",
        "O PJeOffice nao foi identificado na maquina.",
        "Consulte a documentacao oficial do PJeOffice e realize a instalacao manualmente."));
    evaluation.checks.push_back(evidence_check(
        "pjeoffice_execution", input.pjeoffice_running,
        "O PJeOffice esta em execucao.",
        "O PJeOffice foi identificado, mas nao esta em execucao.",
        "Inicie o PJeOffice e repita somente esta verificacao."));
    if (input.uses_external_device) {
        evaluation.checks.push_back(external_device_check(input.external_device_state));
    }
    evaluation.status = aggregate_status(evaluation.checks);
    return evaluation;
}

}  // namespace certradar

#include "certradar/actions.hpp"

namespace certradar {

const std::vector<AllowedAction>& allowed_actions() {
    static const std::vector<AllowedAction> actions{
        {"smartcard_service_start", "Iniciar o servico de Cartao Inteligente e repetir a deteccao.", true, true},
        {"certificate_import_wizard", "Abrir o assistente oficial de importacao do Windows.", false, false},
        {"official_middleware_page", "Abrir a pagina oficial cadastrada para o middleware identificado.", false, false},
    };
    return actions;
}

const AllowedAction* find_allowed_action(const std::string& id) noexcept {
    for (const auto& action : allowed_actions()) {
        if (action.id == id) return &action;
    }
    return nullptr;
}

ActionAuthorization authorize_action(
    const std::string& id,
    const bool explicit_consent) noexcept {
    if (find_allowed_action(id) == nullptr) return ActionAuthorization::unknown_action;
    return explicit_consent ? ActionAuthorization::allowed : ActionAuthorization::consent_required;
}

}  // namespace certradar

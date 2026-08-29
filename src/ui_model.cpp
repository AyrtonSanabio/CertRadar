#include "certradar/ui_model.hpp"

namespace certradar {

std::wstring candidate_state_label(const CandidateState state) {
    switch (state) {
        case CandidateState::extension_match: return L"candidato";
        case CandidateState::recognized: return L"contêiner reconhecido";
        case CandidateState::invalid: return L"arquivo inválido";
        case CandidateState::too_large: return L"acima do limite";
        case CandidateState::inaccessible: return L"inacessível";
    }
    return L"estado desconhecido";
}

std::wstring format_candidate_label(const SearchCandidate& candidate) {
    return L"[" + candidate_state_label(candidate.state) + L"] " + candidate.path.wstring();
}

}  // namespace certradar

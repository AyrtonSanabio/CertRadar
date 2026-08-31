#include "certradar/ui_model.hpp"

#include <array>

namespace certradar {
namespace {

std::wstring scan_status_summary(const ScanStatus status) {
    switch (status) {
        case ScanStatus::not_started: return L"Busca não iniciada";
        case ScanStatus::running: return L"Busca em andamento";
        case ScanStatus::paused: return L"Busca pausada";
        case ScanStatus::completed: return L"Busca concluída";
        case ScanStatus::partial: return L"Busca concluída parcialmente";
        case ScanStatus::cancelled: return L"Busca cancelada com resultados parciais";
        case ScanStatus::failed: return L"Busca encerrada com falha";
    }
    return L"Estado da busca desconhecido";
}

}  // namespace

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

std::wstring build_search_support_summary(const SearchResult& result) {
    std::array<std::uint64_t, 5> states{};
    for (const auto& candidate : result.candidates) {
        const auto index = static_cast<std::size_t>(candidate.state);
        if (index < states.size()) ++states[index];
    }

    std::wstring summary = L"Resumo CertRadar — busca de certificado A1\r\n";
    summary += scan_status_summary(result.status) + L"\r\n";
    summary += L"Candidatos encontrados: " + std::to_wstring(result.candidates.size()) + L"\r\n";
    summary += L"Reconhecidos: " +
               std::to_wstring(states[static_cast<std::size_t>(CandidateState::recognized)]) +
               L"\r\n";
    summary += L"Inválidos: " +
               std::to_wstring(states[static_cast<std::size_t>(CandidateState::invalid)]) +
               L"\r\n";
    summary += L"Acima do limite: " +
               std::to_wstring(states[static_cast<std::size_t>(CandidateState::too_large)]) +
               L"\r\n";
    summary += L"Inacessíveis: " +
               std::to_wstring(states[static_cast<std::size_t>(CandidateState::inaccessible)]) +
               L"\r\n";
    summary += L"Pastas verificadas: " + std::to_wstring(result.directories_visited) + L"\r\n";
    summary += L"Pastas sem acesso: " + std::to_wstring(result.access_denied_count) + L"\r\n";
    summary += L"Erros isolados: " + std::to_wstring(result.errors) + L"\r\n";
    summary += L"Privacidade: nomes e caminhos locais não foram incluídos; nenhum arquivo foi alterado.\r\n";
    return summary;
}

}  // namespace certradar

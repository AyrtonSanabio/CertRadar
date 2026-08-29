#include "certradar/diagnosis.hpp"

namespace certradar {

std::vector<Finding> diagnose(const DiagnosticInput& input) {
    std::vector<Finding> findings;
    if (input.certificate_installed && !input.private_key_associated) {
        findings.push_back({
            "certificate_without_private_key",
            "O certificado publico esta instalado, mas nao ha associacao observavel com chave privada.",
            "Foi importado apenas o certificado publico ou a chave original nao esta disponivel nesta maquina.",
            Confidence::high,
            "Localize o PFX/P12 original ou solicite reemissao; nao exclua o certificado atual."});
    }
    if (!input.smartcard_service_running) {
        findings.push_back({
            "smartcard_service_stopped",
            "O servico de cartao inteligente nao esta em execucao.",
            "O Windows nao consegue comunicar com leitores enquanto o servico estiver parado.",
            Confidence::high,
            "Autorize o inicio do servico e repita somente o teste de leitores."});
    } else if (!input.reader_detected) {
        findings.push_back({
            "reader_not_detected", "Nenhum leitor foi enumerado pelo Windows.",
            "Driver, porta USB ou middleware podem estar indisponiveis.", Confidence::medium,
            "Reconecte o dispositivo e confirme o driver oficial do fabricante."});
    } else if (!input.device_present) {
        findings.push_back({
            "device_absent", "Existe leitor, mas nenhum cartao/token esta presente.",
            "O dispositivo pode estar desconectado ou mal encaixado.", Confidence::high,
            "Insira ou reconecte o token e repita a deteccao."});
    }
    if (!input.a1_file_found && !input.certificate_installed && findings.empty()) {
        findings.push_back({
            "insufficient_evidence", "Nenhum arquivo A1 ou certificado instalado foi localizado.",
            "Nao ha evidencia suficiente para afirmar perda do certificado.", Confidence::low,
            "Amplie a busca local e confirme onde o certificado foi emitido."});
    }
    return findings;
}

}  // namespace certradar

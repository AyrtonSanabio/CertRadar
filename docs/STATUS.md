# Estado da implementação

Atualizado em 31 de agosto de 2026.

| Tarefa | Estado | Evidência |
|---|---|---|
| T000 — Especificação e fluxo acompanhado | Concluída | Commit local `2118cc7` |
| T001 — Estrutura mínima e primeiro teste | Concluída | Commit local `4ed8ada`; x64 e Win32 aprovados |
| T002 — Contrato versionado de fatos | Concluída | Commit local `5566467`; 3 testes aprovados |
| T003 — Detector do Windows | Concluída | Commit local `9ea08b9`; x64 e Win32 aprovados |
| T004 — Classificador de suporte | Concluída | Commit local `1eb2c16`; matriz testada |
| T005 — Pastas prioritárias | Concluída | Commit local `902ffad`; Known Folders testadas |
| T006 — Busca prioritária PFX/P12 | Concluída | Commit local `eed678b`; extensões testadas |
| T007 — Busca iterativa no perfil | Concluída | Commit local `9cf82aa`; fila testada |
| T008 — Permissões e reparse points | Concluída | Commit local `89fa857`; segurança testada |
| T009 — Outros perfis e discos locais | Concluída | Commit local `43eaa32`; rede excluída |
| T010 — Unidades removíveis | Concluída | Commit local `3a8ea57`; fase final testada |
| T011 — Progresso, pausa e cancelamento | Concluída | Commit local `0bcb5b8`; controles testados |
| T012 — Reconhecimento PFX/P12 | Concluída | Commit local `f6079e3`; inspeção testada |
| T013 — Deduplicação determinística | Concluída | Commit local `1cc7abe`; hard link testado |
| T014 — Interface de resultados | Concluída | Commit local `d0a4d0c`; x64/Win32 aprovados |
| T015 — Store pessoal do usuário | Concluída | Commit local `d39d3b9`; store lido |
| T016 — Store pessoal da máquina | Concluída | Commit local `6262e29`; falha isolada |
| T017 — Interpretação X.509 | Concluída | Commit local `eac4508`; campos testados |
| T018 — Associação de chave privada | Concluída | Commit local `1101b91`; sem adquirir chave |
| T019 — Cadeia local | Concluída | Commit local `909d2d3`; erros classificados |
| T020 — Revogação online | Concluída | Commit local `a176469`; timeout testado |
| T021 — Serviço de cartão inteligente | Concluída | Commit local `fccdb74`; estado testado |
| T022 — Leitores e cartões | Concluída | Commit local `281fd03`; WinSCard testado |
| T023 — Provider e middleware | Concluída | Commit local `246dde1`; CSP/KSP testados |
| T024 — Estados separados de A3 | Concluída | Commit local `2b2adf7`; estados testados |
| T025 — Assinatura sintética A1 | Concluída | Commit local `76e16a3`; RSA efêmero aprovado |
| T026 — Assinatura A3 autorizada | Parcial, aguardando hardware | Barreira de consentimento testada; assinatura real exige A3 |
| T027 — Motor de diagnóstico | Concluída | Regras puras produzem conclusões testáveis |
| T028 — Fato, causa, confiança e ação | Concluída | Estrutura explicável coberta por testes |
| T029 — Relatório local mascarado | Concluída | JSON local-only e mascaramento testados |
| T030 — Resumo para chamado | Concluída | Texto copiável com próxima ação |
| T031 — Executor allowlist | Concluída | IDs compilados recusam texto arbitrário e exigem consentimento |
| T032 — Ação segura no serviço | Concluída | Consentimento, início controlado e reteste limitado |
| T033 — Assistente oficial de importação | Concluída | CryptoUI interativa, PFX reconhecido e consentimento |
| T034 — Middleware oficial | Concluída | Catálogo fechado, consentimento e somente páginas HTTPS oficiais |
| T035 — Comparação antes/depois | Concluída | Reteste seletivo classifica resolução, regressão e evidência incompleta |
| T036 — Perfil de aplicação PJe | Concluída | Certificado, chave, cadeia, PJeOffice e A3 avaliados separadamente |
| T037 — Pacote portátil Windows 10/11 | Concluída | Release x64/x86, runtime estático, hashes, PE e smoke test no Windows 10 |
| T038 — Fluxo completo em VMs Windows 10/11 | Parcial, aguardando VMs | Coletor sanitizado pronto; execução manual Win10/11 pendente |
| T039 — Modo compatível Windows 7 SP1/8.1 | Aguardando VMs | Coletor pronto, sem evidência de runtime nesses sistemas |
| T040 — Modo legado básico Windows XP SP3 | Parcial, aguardando VM | PE32/subsistema 5.01 e busca básica aprovados no host; runtime XP não comprovado |
| T041 — Fuzzing e análise estática | Concluída | 10.500 mutações determinísticas, 56 testes x64/Win32 e clang-tidy em 16 fontes |
| T042 — Auditoria de privacidade | Concluída | Sentinelas redigidos, 58 testes x64/Win32 e auditoria de APIs/arquivos |
| T043 — Release candidate reproduzível | Bloqueada por portões externos | VMs, A3 real, licença e assinatura de código pendentes |
| T044 — Resumo sanitizado da busca | Concluída | Resumo sem nomes/caminhos e 59 testes aprovados em x64/Win32 |
| T045 — Localização segura do arquivo | Concluída | Plano recusa seleção/caminho inválido e 60 testes aprovados em x64/Win32 |
| T046 — Ambiente visível na interface | Concluída | Windows/build/arquitetura/suporte/privilégio e 61 testes aprovados em x64/Win32 |
| T047 — Ambiente no resumo do chamado | Concluída | Mesmos fatos detectados entram no resumo sem nomes/caminhos; 61 testes x64/Win32 |
| T048 — Certificados instalados na interface | Concluída | Store pessoal do usuário lido em segundo plano; 62 testes x64/Win32 |

## Tarefa em revisão

### T048 — Certificados instalados na interface

Entregue:

- botão **Instalados** para consultar o store pessoal do usuário atual;
- enumeração somente leitura em thread de trabalho, sem congelar a janela;
- rótulos com validade, vínculo aparente com chave, tipo de provider, vencimento e somente o final da impressão digital;
- omissão de titular, emissor, série, provider completo e impressão digital completa;
- isolamento de falhas de acesso sem derrubar a aplicação.

Não entregue:

- leitura do store da máquina pela interface;
- aquisição, teste, exportação ou uso da chave privada;
- abertura de assistentes, solicitação de PIN ou alteração do store;
- cópia desse resultado para o chamado;
- qualificação pendente nas VMs alvo e promoção para release pública.

## Comando de prova

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

Resultado atual:

```text
100% tests passed, 0 tests failed out of 62
```

O empacotamento Release executa a suíte, valida os hashes e inicia o binário a partir da pasta portátil. O binário permanece explicitamente não assinado.

## Regime de execução

O proprietário suspendeu as pausas entre checkpoints em 28 de agosto de 2026. A implementação continuará em sequência, sem `push`, preservando testes, documentação e commits locais como pontos de estudo.

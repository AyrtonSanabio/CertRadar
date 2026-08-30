# Estado da implementação

Atualizado em 30 de agosto de 2026.

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
| T035 — Comparação antes/depois | Próxima | Execução contínua autorizada |

## Tarefa em revisão

### T001 — Estrutura mínima e primeiro teste

Entregue:

- declaração pública mínima do núcleo;
- implementação da identidade `CertRadar`;
- teste automatizado do comportamento;
- configuração CMake reproduzível e scripts de compilação/teste;
- warnings do compilador tratados como erro;
- dependência doctest fixada por commit;
- builds modernos separados em x64 e Win32.

Não entregue:

- interface;
- detecção do Windows;
- busca de certificados;
- contrato de diagnóstico;
- compatibilidade comprovada com versões antigas.

## Comando de prova

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

Resultado atual:

```text
100% tests passed, 0 tests failed out of 44
```

## Regime de execução

O proprietário suspendeu as pausas entre checkpoints em 28 de agosto de 2026. A implementação continuará em sequência, sem `push`, preservando testes, documentação e commits locais como pontos de estudo.

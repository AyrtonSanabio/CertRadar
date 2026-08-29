# Estado da implementação

Atualizado em 28 de agosto de 2026.

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
| T010 — Unidades removíveis | Concluída | Removíveis entram por último; falhas ficam isoladas |
| T011 — Progresso, pausa e cancelamento | Próxima | Execução contínua autorizada |

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
100% tests passed, 0 tests failed out of 1
```

## Regime de execução

O proprietário suspendeu as pausas entre checkpoints em 28 de agosto de 2026. A implementação continuará em sequência, sem `push`, preservando testes, documentação e commits locais como pontos de estudo.

# Estado da implementação

Atualizado em 28 de agosto de 2026.

| Tarefa | Estado | Evidência |
|---|---|---|
| T000 — Especificação e fluxo acompanhado | Concluída | Commit local `2118cc7` |
| T001 — Estrutura mínima e primeiro teste | Em finalização | Teste aprovado em MSVC 2022 x64 e Win32 |
| T002 — Contrato versionado de fatos | Próxima | Execução contínua autorizada |

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

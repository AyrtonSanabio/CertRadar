# Roadmap

O roadmap é organizado por evidência, não por prazo. Um marco somente termina quando seus critérios forem comprovados.

## M0 — Especificação e revisão

Estado: **concluído e registrado no commit local `2118cc7`**.

Entregas:

- visão do produto;
- requisitos identificados;
- arquitetura candidata;
- busca segura;
- ameaça e privacidade;
- estratégia TDD;
- decisões abertas.

Saída: aprovação do escopo ou lista de correções documentais.

A execução detalhada será feita pelos checkpoints T000–T043 definidos em [10_FLUXO_DE_IMPLEMENTACAO_ACOMPANHADA.md](10_FLUXO_DE_IMPLEMENTACAO_ACOMPANHADA.md). Cada checkpoint terá pausa obrigatória para estudo e aprovação.

## M1 — Experimentos de compatibilidade

Entregas:

- launcher mínimo nas VMs alvo;
- enumeração de stores;
- busca cancelável;
- WinSCard sem prompt indevido;
- contrato JSON;
- prova de isolamento de falha.

Saída: decisão registrada sobre linguagens, toolchains, módulos e distribuição.

## M2 — MVP somente leitura

Entregas:

- detecção do Windows;
- busca prioritária e completa;
- inventário de certificados;
- validade e associação com chave;
- leitor/token/serviço/provider;
- diagnóstico inicial;
- relatório local sanitizado.

Saída: conjunto de cenários de laboratório aprovado em todas as VMs declaradas.

## M3 — Teste funcional de assinatura

Entregas:

- desafio aleatório local;
- autorização e UI segura do provider;
- verificação local;
- descarte de dados;
- tratamento de cancelamento e PIN bloqueado sem consumir tentativas indevidas.

Saída: testes com A1 sintético e amostra controlada de tokens.

## M4 — Correções seguras

Entregas:

- executor separado;
- lista permitida;
- consentimento por ação;
- reteste e comparação;
- reversão quando aplicável.

Saída: nenhuma ação destrutiva e todas as ações cobertas por testes de integração.

## M5 — Perfis de aplicações

Escolher o primeiro perfil usando volume real de chamados. Candidatos:

- PJe/PJeOffice;
- e-CAC;
- assinatura de PDF;
- emissão fiscal.

Saída: checklist específico, coleta mínima de logs e diagnóstico testado.

## M6 — Endurecimento e piloto

Entregas:

- assinatura de código;
- pipeline de build e testes;
- análise estática e fuzzing;
- instalador/pacote portátil validado;
- piloto supervisionado;
- métricas sem telemetria invasiva;
- processo de vulnerabilidade.

Saída: decisão de primeira versão pública.

## Futuro fora do compromisso inicial

- código temporário para suporte remoto assistido;
- integração com sistema de chamados;
- monitoramento preventivo de vencimento;
- busca dentro de arquivos compactados;
- inventário corporativo;
- suporte a novos tipos da modernização ICP-Brasil.

Nenhum desses itens deverá atrasar o MVP de diagnóstico local.

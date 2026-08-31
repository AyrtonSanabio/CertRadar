# Fluxo de implementação acompanhada

## Objetivo

O CertRadar será implementado em checkpoints pequenos para que o proprietário possa abrir o projeto, estudar a mudança, executar os testes e compreender a decisão antes de a próxima funcionalidade começar.

Neste projeto, “checkpoint” é o equivalente ao endpoint de aprendizado citado durante o planejamento. Como o CertRadar é uma aplicação desktop, nem todo checkpoint será um endpoint HTTP; cada um será um comportamento observável e testável.

## Regra principal

> Uma tarefa por vez. Depois de finalizar, o Codex para. A próxima tarefa somente começa após aprovação explícita do proprietário.

### Exceção ativa: execução contínua

Em 28 de agosto de 2026, o proprietário autorizou explicitamente a execução contínua de tudo que está documentado. Durante esta execução:

- as pausas para estudo e as aprovações intermediárias ficam suspensas;
- a ordem dos checkpoints, o ciclo TDD e as revisões de segurança continuam obrigatórios;
- cada marco deve continuar rastreável por testes, documentação e commits locais;
- nenhuma alteração será enviada ao GitHub sem uma solicitação explícita de `push`.

Ao término, o proprietário poderá estudar o histórico completo tarefa por tarefa.

Não será permitido:

- juntar duas funcionalidades apenas porque são próximas;
- começar a tarefa seguinte enquanto a atual está em revisão;
- esconder refatorações grandes dentro de uma tarefa pequena;
- criar commit antes da aprovação do proprietário;
- declarar concluído sem teste e prova reproduzível;
- corrigir problemas fora do escopo sem autorização.

## Estados de uma tarefa

```text
Pendente
   ↓
Em implementação
   ↓
Pronta para estudo
   ├── correções solicitadas ──→ Em implementação
   ↓ aprovação explícita
Aprovada
   ↓ autorização para commit
Commitada
   ↓
Concluída
```

Somente uma tarefa poderá estar “Em implementação” ou “Pronta para estudo”.

## Antes de codificar uma tarefa

O Codex deverá informar:

```text
Vou iniciar a Tarefa TXXX — Nome

Objetivo:
Comportamento observável que será entregue.

Arquivos previstos:
Arquivos que provavelmente serão criados ou alterados.

Testes que escreverei primeiro:
Cenários que devem falhar antes da implementação.

Fora do escopo:
O que não será feito nesta tarefa.
```

Se a tarefa crescer durante a implementação, ela deverá ser interrompida e dividida antes de continuar.

## Ciclo obrigatório de implementação

### 1. Ler a documentação

Ler o requisito, segurança, arquitetura e decisões relacionadas. A documentação é a fonte de verdade.

### 2. Escrever o teste

Criar o teste do comportamento público antes da implementação.

### 3. Confirmar a falha

Executar o teste e confirmar que ele falha pelo motivo esperado. Uma falha causada por erro de compilação acidental ou configuração incorreta não vale como etapa vermelha do TDD.

### 4. Implementar o mínimo

Escrever somente o código necessário para fazer o teste passar, preservando os limites de segurança.

### 5. Executar testes

Executar o teste novo, testes do módulo e regressão proporcional ao risco.

### 6. Refatorar

Melhorar nomes, estrutura e duplicações sem adicionar nova funcionalidade.

### 7. Revisar segurança

Verificar privilégios, dados pessoais, logs, caminhos, cancelamento, erros e possibilidade de acesso à chave privada.

### 8. Atualizar documentação

Atualizar requisitos, changelog, roadmap e instruções de estudo quando necessário.

### 9. Entregar para estudo

Parar o desenvolvimento e apresentar o relatório padronizado.

## Relatório obrigatório ao finalizar

Toda tarefa terminará com esta estrutura:

```text
Finalizei a Tarefa TXXX — Nome

O que ficou pronto:
- comportamento entregue;
- comportamento não entregue.

Como estudar:
1. primeiro arquivo e conceito;
2. segundo arquivo e fluxo;
3. teste que demonstra o comportamento.

Como executar:
comando exato para build/teste/demonstração.

Prova:
- testes executados;
- aprovados, falhos e ignorados;
- sistema em que foram executados.

Decisões importantes:
- por que a solução foi estruturada dessa forma.

Limitações conhecidas:
- o que ainda não funciona ou pertence a outra tarefa.

Estado do Git:
- arquivos alterados;
- nenhum commit criado, aguardando revisão.

Próxima tarefa proposta:
TYYY — Nome, ainda não iniciada.
```

Além disso, os arquivos relevantes deverão ser indicados com linhas para facilitar a leitura.

## Papel do proprietário durante a revisão

O proprietário poderá:

1. abrir o projeto;
2. seguir a ordem de estudo indicada;
3. executar os testes;
4. fazer perguntas sobre qualquer linha;
5. pedir simplificação ou correção;
6. aprovar a tarefa;
7. autorizar o commit e a próxima tarefa.

Respostas válidas podem ser simples:

```text
Aprovado. Pode commitar e seguir para a próxima.
```

ou:

```text
Não aprovei. Explique/corrija a parte X.
```

Sem uma aprovação explícita, o Codex permanecerá na tarefa atual.

## Estratégia de Git

- A branch `main` conterá apenas checkpoints aprovados.
- Durante a implementação, as alterações permanecerão sem commit para revisão.
- Depois da aprovação, o Codex mostrará o diff final e pedirá ou usará a autorização já dada para criar um commit da tarefa.
- Cada commit representará um checkpoint, salvo ajustes documentais inseparáveis.
- Mensagem sugerida: `tipo(escopo): conclui TXXX descricao curta`.
- Não será feito push sem pedido explícito.

## Plano completo de checkpoints

Os números reservam uma ordem de estudo. Eles poderão ser divididos, mas não agrupados, se a implementação revelar complexidade maior.

### Fase A — Aprovação e fundação

| Tarefa | Entrega observável | Conceito principal para estudo |
|---|---|---|
| T000 | Aprovação da especificação e commit documental inicial | Escopo, requisitos e fonte de verdade |
| T001 | Solução mínima compila e um teste vazio é executado | Estrutura do projeto e test runner |
| T002 | Contrato versionado de fatos do diagnóstico | Modelagem de dados e serialização |
| T003 | Detector informa versão e arquitetura do Windows | APIs do sistema e abstração |
| T004 | Classificador seleciona modo completo, compatível ou legado | Regras puras e tabela de compatibilidade |

### Fase B — Busca de arquivos A1

| Tarefa | Entrega observável | Conceito principal para estudo |
|---|---|---|
| T005 | Resolve Downloads, Área de Trabalho e Documentos pelas APIs do Windows | Known folders e ausência de caminhos fixos |
| T006 | Encontra PFX/P12 nas pastas prioritárias | Enumeração de arquivos e comparação de extensão |
| T007 | Percorre iterativamente o perfil atual | Fila, iteração e controle de memória |
| T008 | Continua após acesso negado e não entra em junções/loops | Permissões e reparse points |
| T009 | Percorre demais perfis e discos locais acessíveis | Raízes, escopo e isolamento por unidade |
| T010 | Percorre unidades removíveis e tolera desconexão | Dispositivos transitórios e tratamento de falhas |
| T011 | Expõe progresso, pausa e cancelamento | Concorrência cooperativa |
| T012 | Reconhece estrutura PFX/P12 sem pedir senha | Parsing mínimo e limites de tamanho |
| T013 | Deduplica resultados e mantém ordem de descoberta | Identidade de arquivo e determinismo |
| T014 | Exibe resultados da busca na interface | Separação entre UI e núcleo |

### Fase C — Certificados instalados

| Tarefa | Entrega observável | Conceito principal para estudo |
|---|---|---|
| T015 | Lista certificados pessoais do usuário atual | X509 store Current User |
| T016 | Lista certificados pessoais da máquina quando acessíveis | Local Machine e privilégios |
| T017 | Interpreta titular, emissor, uso e validade | X.509 e mascaramento |
| T018 | Distingue certificado com e sem associação de chave privada | Certificado público versus credencial |
| T019 | Constrói cadeia e explica erros locais | Cadeia de confiança |
| T020 | Consulta revogação com timeout e estado indeterminado | Rede, CRL/OCSP e falha segura |

### Fase D — A3, token e teste funcional

| Tarefa | Entrega observável | Conceito principal para estudo |
|---|---|---|
| T021 | Informa estado do serviço de cartão inteligente | Serviços do Windows |
| T022 | Enumera leitores e presença de cartão/token | WinSCard |
| T023 | Identifica provider e middleware quando possível | CSP, KSP e isolamento legado |
| T024 | Diagnostica driver, dispositivo e chave como estados separados | Máquina de estados |
| T025 | Assina e verifica desafio local com A1 sintético | Assinatura digital sem documento real |
| T026 | Assina e verifica desafio com A3 após autorização | PIN seguro e operação no dispositivo |

### Fase E — Diagnóstico e suporte

| Tarefa | Entrega observável | Conceito principal para estudo |
|---|---|---|
| T027 | Motor recebe fatos e produz uma conclusão testável | Regras de diagnóstico |
| T028 | Resultado separa fato, causa, confiança e ação | Explicabilidade |
| T029 | Relatório local mascara dados sensíveis | Privacidade e minimização |
| T030 | Resumo pode ser copiado para o chamado | Formato de suporte |
| T031 | Executor aceita somente ações cadastradas | Allowlist e separação de privilégios |
| T032 | Ação segura inicia/reinicia serviço de cartão com consentimento | Mudança controlada e reteste |
| T033 | Ação abre o assistente oficial de importação | Delegação para UI segura do Windows |
| T034 | Ação direciona ao middleware oficial identificado | Origem, assinatura e confiança |
| T035 | Comparação mostra o antes e o depois da correção | Evidência e regressão localizada |
| T036 | Primeiro perfil de aplicação é implementado | Diagnóstico específico; perfil ainda será escolhido |

### Fase F — Compatibilidade, endurecimento e entrega

| Tarefa | Entrega observável | Conceito principal para estudo |
|---|---|---|
| T037 | Pacote portátil inicia no Windows 10/11 | Empacotamento e assinatura |
| T038 | Fluxo completo aprovado em VMs Windows 10/11 | Testes de sistema |
| T039 | Modo compatível aprovado em Windows 7 SP1/8.1 | Degradação controlada |
| T040 | Modo legado básico aprovado em XP SP3 | Compatibilidade sem promessas falsas |
| T041 | Fuzzing e análise estática cobrem entradas hostis | Robustez e memória |
| T042 | Auditoria de privacidade confirma ausência de segredos | Segurança de relatórios e logs |
| T043 | Release candidate reproduzível e documentada | Processo de release |

### Fase G — Usabilidade operacional

Esta fase começou após a implementação do roteiro original. Ela integra ao executável comportamentos já existentes no núcleo sem enfraquecer os portões de release da Fase F.

| Tarefa | Entrega observável | Conceito principal para estudo |
|---|---|---|
| T044 | Copia um resumo sanitizado da busca para o chamado | Fronteira entre dado local e dado compartilhável |
| T045 | Mostra no Explorer o candidato selecionado | Planejamento puro e efeito Win32 controlado |
| T046 | Exibe o ambiente e o nível de suporte detectados | Adaptação de fatos técnicos para linguagem operacional |
| T047 | Inclui o ambiente detectado no resumo da busca | Reuso de fatos sem duplicar detecção |
| T048 | Lista na janela os certificados instalados do usuário atual | Store X.509 assíncrono e minimização visual |
| T049 | Copia contagens sanitizadas dos certificados instalados | Agregação operacional sem identidade |

## Tarefa atual

**T049 — Resumo sanitizado dos certificados instalados, concluída.**

O roteiro T000–T043 permanece preservado. A Fase G reúne integrações incrementais de usabilidade autorizadas pela continuação do desenvolvimento; cada tarefa mantém TDD, documentação e commit próprio.

## Alteração deste plano

Novos conhecimentos poderão exigir divisão ou reordenação. Toda alteração deverá:

1. ser explicada antes de codificar;
2. preservar dependências e segurança;
3. atualizar este documento;
4. ser aprovada pelo proprietário quando mudar escopo ou ordem de aprendizado.

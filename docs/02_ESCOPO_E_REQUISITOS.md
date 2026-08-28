# Escopo e requisitos

Os identificadores abaixo deverão ser usados em testes, issues e pull requests.

## Plataformas

### SUP-001 — Suporte completo

Windows 10 e Windows 11, nas arquiteturas x86 e x64 quando existentes. Windows em ARM poderá ser detectado, mas não será declarado totalmente compatível até que tokens e middlewares reais sejam testados.

### SUP-002 — Suporte compatível

Windows 7 SP1 e Windows 8.1, x86 e x64. O programa deverá priorizar diagnóstico e ações guiadas, apresentando aviso de sistema fora de suporte.

### SUP-003 — Modo legado

Windows XP SP3, inicialmente x86. O modo legado deverá realizar apenas verificações comprovadamente seguras e informar incompatibilidades sem prometer correção.

### SUP-004 — Fora do escopo

macOS, Linux, Windows Server, Windows Vista, Windows 8 sem 8.1 e versões anteriores ao XP SP3.

## Requisitos funcionais

### Ambiente

- **RF-ENV-001:** detectar versão, edição, service pack, build e arquitetura do Windows.
- **RF-ENV-002:** detectar se o processo está elevado e nunca exigir elevação apenas para diagnóstico básico.
- **RF-ENV-003:** identificar discos locais e removíveis sem incluir compartilhamentos de rede por padrão.
- **RF-ENV-004:** verificar data, hora e fuso, sem alterá-los automaticamente.

### Busca de arquivos

- **RF-BUS-001:** procurar `.pfx` e `.p12`, sem diferenciar maiúsculas e minúsculas.
- **RF-BUS-002:** começar por Downloads, Área de Trabalho e Documentos.
- **RF-BUS-003:** continuar pelo perfil atual, demais perfis acessíveis, discos locais e unidades removíveis.
- **RF-BUS-004:** percorrer todas as pastas locais acessíveis quando a busca prioritária não for suficiente.
- **RF-BUS-005:** não tentar contornar ACLs ou permissões do Windows.
- **RF-BUS-006:** não seguir pontos de nova análise, junções ou links que possam causar ciclos.
- **RF-BUS-007:** permitir pausar ou cancelar e liberar recursos rapidamente.
- **RF-BUS-008:** mostrar fase atual, local aproximado e contadores sem expor caminhos sensíveis no relatório padrão.
- **RF-BUS-009:** detectar duplicidades por identidade de arquivo e, quando necessário, por hash calculado sob demanda.
- **RF-BUS-010:** tratar arquivos inacessíveis, removidos durante a busca ou malformados sem interromper a varredura.
- **RF-BUS-011:** nunca pedir senha durante a simples localização.

### Certificados instalados

- **RF-CER-001:** enumerar os repositórios pessoais do usuário atual e da máquina quando acessíveis.
- **RF-CER-002:** apresentar titular mascarado, emissor, série mascarada, início e fim da validade, impressão digital e usos declarados.
- **RF-CER-003:** identificar associação aparente com chave privada sem exportá-la.
- **RF-CER-004:** construir a cadeia e separar erros de validade, confiança, política e revogação.
- **RF-CER-005:** distinguir certificado público presente de credencial efetivamente utilizável.
- **RF-CER-006:** identificar certificados vencidos, ainda não válidos, revogados quando consultável e próximos do vencimento.

### A3, tokens e cartões

- **RF-TOK-001:** enumerar leitores e cartões visíveis pelas APIs do Windows.
- **RF-TOK-002:** detectar o estado do serviço de cartão inteligente.
- **RF-TOK-003:** identificar provedores CSP/KSP e middleware quando tecnicamente possível.
- **RF-TOK-004:** diferenciar “driver instalado”, “dispositivo detectado” e “chave utilizável”.
- **RF-TOK-005:** não consumir tentativas de PIN durante diagnóstico silencioso.

### Teste funcional

- **RF-TST-001:** oferecer teste opcional de assinatura de um desafio aleatório local.
- **RF-TST-002:** solicitar autorização antes de qualquer operação que possa apresentar janela de PIN.
- **RF-TST-003:** verificar localmente a assinatura produzida.
- **RF-TST-004:** descartar o desafio e a assinatura ao finalizar, salvo se o usuário pedir evidência técnica sem dados pessoais.
- **RF-TST-005:** nunca capturar PIN ou senha em campo controlado pelo CertRadar quando houver diálogo seguro do sistema ou fabricante.

### Diagnóstico

- **RF-DIA-001:** produzir conclusões baseadas em fatos observáveis, com nível de confiança.
- **RF-DIA-002:** separar fato, causa provável e ação recomendada.
- **RF-DIA-003:** explicar quando não há evidência suficiente para uma conclusão.
- **RF-DIA-004:** adaptar regras à versão do Windows e ao modo de suporte.
- **RF-DIA-005:** permitir selecionar o objetivo, como PJe, e-CAC ou assinatura de PDF, quando esses perfis forem implementados.

### Correções

- **RF-COR-001:** toda alteração deverá exigir autorização explícita e descrever impacto.
- **RF-COR-002:** ações deverão pertencer a uma lista permitida e versionada.
- **RF-COR-003:** ações reversíveis deverão registrar como desfazer.
- **RF-COR-004:** após uma ação, repetir apenas os testes afetados e comparar antes/depois.
- **RF-COR-005:** não instalar silenciosamente raízes, drivers, middleware ou software de terceiros.
- **RF-COR-006:** não excluir certificados, chaves ou drivers na primeira versão.

### Relatório e atendimento

- **RF-REL-001:** gerar resumo legível para cliente e seção técnica para o atendente.
- **RF-REL-002:** mascarar CPF, CNPJ, números de série, nomes e caminhos por padrão.
- **RF-REL-003:** permitir copiar o resumo para um chamado.
- **RF-REL-004:** registrar testes, resultados, ações autorizadas e retestes.
- **RF-REL-005:** manter o relatório somente na máquina até autorização explícita de compartilhamento.

## Requisitos não funcionais

- **RNF-SEG-001:** operar com privilégio mínimo.
- **RNF-SEG-002:** não possuir telemetria na primeira versão.
- **RNF-SEG-003:** não executar comandos construídos por concatenação de caminhos não confiáveis.
- **RNF-SEG-004:** validar assinatura digital e origem de componentes e atualizações antes de executá-los.
- **RNF-CON-001:** a interface não poderá bloquear durante busca, validação online ou leitura de dispositivo.
- **RNF-CON-002:** todas as operações longas deverão possuir timeout e cancelamento.
- **RNF-CON-003:** uma falha em um arquivo, pasta, certificado ou provider não deverá encerrar a sessão.
- **RNF-CON-004:** resultados deverão ser determinísticos para a mesma captura de ambiente, salvo dados externos identificados como variáveis.
- **RNF-PRI-001:** coletar somente dados necessários ao diagnóstico.
- **RNF-PRI-002:** não armazenar conteúdo de documentos do usuário.
- **RNF-PRI-003:** dados temporários deverão ser removidos ao encerrar normalmente e na inicialização seguinte após encerramento inesperado.
- **RNF-USA-001:** mensagens deverão dizer o que ocorreu, por que importa e qual é a próxima ação.
- **RNF-USA-002:** o usuário deverá poder sair sem aplicar nenhuma alteração.

## Critério geral de aceite

Uma funcionalidade somente estará pronta quando:

1. o requisito estiver documentado;
2. existirem testes automatizados anteriores ou simultâneos à implementação;
3. os testes relevantes passarem nas plataformas declaradas;
4. riscos e tratamento de dados estiverem revisados;
5. erros forem apresentados sem revelar dados sensíveis;
6. a documentação do usuário e do suporte estiver atualizada;
7. houver prova reproduzível do resultado.

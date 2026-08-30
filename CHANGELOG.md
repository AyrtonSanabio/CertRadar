# Changelog

Todas as mudanças relevantes deste projeto serão registradas aqui.

O formato segue a ideia de [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/), sem assumir versionamento de produto antes da primeira implementação.

## Não lançado

### Adicionado

- Visão e proposta de valor do CertRadar.
- Escopo fechado para Windows 10/11, Windows 7 SP1/8.1 e XP SP3 em modo legado.
- Requisitos funcionais e não funcionais identificados.
- Arquitetura candidata com isolamento entre coleta, diagnóstico e ações.
- Especificação da busca progressiva em todas as pastas locais acessíveis.
- Modelo de segurança, privacidade e minimização de dados.
- Estratégia TDD, matriz de plataformas e portões de entrega.
- Fluxo de suporte, roadmap, glossário e referências oficiais.
- Política inicial de segurança e guia de contribuição.
- Fluxo acompanhado com checkpoints T000–T043, relatório padronizado de conclusão, pausa para estudo e aprovação obrigatória antes de avançar.
- Estrutura mínima em C++17 para a T001, com identidade pública do núcleo, testes doctest e scripts CMake/PowerShell validados em x64 e Win32.
- Modo contínuo autorizado pelo proprietário: checkpoints, testes, documentação e commits locais permanecem obrigatórios, mas as pausas entre tarefas foram suspensas.
- Contrato de fatos JSON versão 1 com round trip, rejeição de versões incompatíveis e dependência nlohmann/json verificada por SHA-256.
- Detector nativo de versão real do Windows, arquitetura do sistema, edição, service pack e estado de elevação, sem solicitar privilégios.
- Classificador puro da matriz de suporte para Windows 10/11, 7 SP1, 8.1 e XP SP3, com recusa explícita das versões fora do escopo.
- Resolução de Downloads, Área de Trabalho e Documentos pela Known Folders API, preservando a ordem de busca do suporte.
- Busca inicial determinística por arquivos `.pfx` e `.p12`, sem diferenciar maiúsculas e sem abrir ou executar candidatos.
- Varredura iterativa em largura para perfis com árvores profundas, sem recursão da pilha.
- Contenção de falhas por diretório, contagem separada de acesso negado e recusa de junções/reparse points para impedir ciclos.
- Plano de busca por fases para perfil atual, perfis irmãos acessíveis e discos fixos, excluindo unidades de rede por padrão.
- Unidades removíveis entram somente na fase final; mídias ópticas e caminhos remotos continuam fora do escopo padrão.
- Progresso incremental, pausa e cancelamento cooperativos, preservando resultados parciais e liberando a busca sem forçar threads.
- Reconhecimento estrutural PKCS#12 pela CryptoAPI sem pedir senha, com limite padrão de 32 MiB e classificação segura de arquivos inválidos.
- Deduplicação por identidade de volume/arquivo, incluindo hard links e raízes repetidas, mantendo a primeira ocorrência na ordem de descoberta.
- Primeira interface nativa responsiva com início explícito, pausa, continuação, cancelamento, progresso e lista local de candidatos classificados.
- Enumeração somente leitura de certificados pessoais do usuário atual, mantendo apenas o certificado público codificado para análises locais posteriores.
- Leitura opcional do store pessoal da máquina sem solicitar elevação; acesso indisponível retorna código observável em vez de abortar o diagnóstico.
- Interpretação X.509 de titular, emissor, série, impressão digital, período UTC, validade e usos estendidos, com alerta de vencimento em 30 dias.
- Detecção aparente de chave privada pela propriedade de provider do certificado, sem abrir container, exportar chave ou provocar PIN.
- Construção de cadeia usando apenas cache local e explicação separada de validade, raiz não confiável, cadeia parcial, revogação e restrições.
- Validação online opcional com timeout explícito, checagem de revogação da cadeia sem raiz e distinção entre revogado e consulta indeterminada.
- Consulta somente leitura do estado do serviço de cartão inteligente, distinguindo parado, pendente, ausente e inacessível.
- Enumeração WinSCard de leitores e presença do cartão/token com timeout zero, sem abrir transação nem consumir tentativa de PIN.
- Identificação do nome de middleware/provider associado ao certificado e classificação entre CSP legado e KSP moderno.
- Máquina de estados A3 separando serviço, leitor, dispositivo, middleware e associação de chave, evitando diagnósticos genéricos de “token não funciona”.
- Prova funcional sintética com chave RSA efêmera, desafio aleatório, assinatura e verificação locais; buffers sensíveis são zerados e nada é persistido.
- Barreira testável de consentimento para operações com token externo; a prova de assinatura A3 permanece pendente até existir hardware/middleware de laboratório.
- Motor de diagnóstico puro com conclusões explicáveis por fato, causa provável, confiança e ação, incluindo resposta de evidência insuficiente.
- Relatório JSON local e resumo copiável para chamado, com mascaramento de identificadores e caminhos e declaração explícita de ausência de chave privada.
- Allowlist compilada de ações corretivas com IDs estáveis, descrição de impacto e bloqueio obrigatório sem consentimento; comandos arbitrários são recusados.
- Ação controlada para iniciar `SCardSvr`: não altera serviço já ativo, exige consentimento, usa Service Control Manager diretamente e confirma o estado depois da mudança.
- Abertura do assistente oficial `CryptUIWizImport` apenas para PFX/P12 reconhecido e autorizado, sem senha controlada pelo CertRadar e sem comando de shell.
- Catálogo fechado de provedores conhecidos que, após consentimento, direciona o suporte às páginas HTTPS oficiais de SafeSign, SafeNet/eToken e Watchdata; provedores desconhecidos são recusados e nenhum instalador é baixado ou executado automaticamente.
- Plano de reteste seletivo por ação autorizada e comparação estruturada antes/depois, distinguindo problema resolvido, regressão, estado inalterado e evidência incompleta sem reexecutar verificações não relacionadas.
- Primeiro perfil de aplicação para PJe, com avaliação independente de certificado, validade, chave privada, cadeia, PJeOffice e dispositivo A3; o resultado local não promete disponibilidade do tribunal nem sucesso do login.
- Pipeline de pacote portátil Release x64/x86 com runtime C++ estático, manifesto `asInvoker`, metadados de versão, allowlist de arquivos, SHA-256, validação de arquitetura PE e smoke test de inicialização; os artefatos continuam explicitamente não assinados.
- Coletor sanitizado de evidências para qualificação em VMs, com validação automatizada de pacote/inicialização e checklist manual obrigatoriamente pendente; nenhuma compatibilidade é declarada sem execução no sistema alvo.
- Coletor legado isolado em C++98 para XP x86, limitado à busca iterativa por nomes PFX/P12; build PE32/subsistema 5.01, imports e pacote `legacy-untested` são verificados sem alegar execução comprovada no XP.
- Campanhas determinísticas com 10.500 entradas hostis e análise `clang-tidy` dos 16 fontes modernos; a revisão removeu garantias `noexcept` inseguras e explicitou ramos padrão na janela Win32.

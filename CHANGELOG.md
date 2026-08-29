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

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

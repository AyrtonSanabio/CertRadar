# CertRadar

> Encontra o certificado, identifica o problema e orienta a solução.

O **CertRadar** será um assistente portátil de suporte a certificados digitais no Windows. A ferramenta deverá localizar arquivos A1, inspecionar certificados instalados, verificar validade, associação com chave privada, cadeia de confiança, tokens e middleware, executar testes seguros e produzir orientações claras para reduzir tarefas manuais durante o atendimento.

## Estado atual

**Fase: implementação incremental, ainda sem release assinada.**

O repositório contém um núcleo nativo em C++17, testes automatizados x64/Win32 e uma interface de desenvolvimento para busca segura de certificados A1. Os módulos de coleta, diagnóstico, ações autorizadas, reteste e o primeiro perfil PJe já estão em construção; a matriz de VMs e o empacotamento ainda não foram concluídos.

## Proposta de valor

O CertRadar deverá responder a uma pergunta de maneira rápida e reproduzível:

> O certificado está utilizável nesta máquina? Se não, qual é a causa provável e qual é a próxima ação segura?

Para o suporte, isso significa:

- reduzir verificações manuais e repetitivas;
- padronizar o diagnóstico entre atendentes;
- resolver casos comuns sem controle remoto completo;
- testar novamente após uma correção;
- gerar um resumo pronto para o chamado.

## Escopo de sistemas operacionais

| Nível | Sistemas | Compromisso proposto |
|---|---|---|
| Completo | Windows 10 e Windows 11 | Diagnóstico, teste funcional e correções seguras autorizadas |
| Compatível | Windows 7 SP1 e Windows 8.1 | Diagnóstico amplo, com correções mais restritas e alertas de segurança |
| Legado | Windows XP SP3 | Diagnóstico básico e identificação de incompatibilidades |

Não fazem parte do escopo: macOS, Linux, Windows Server, Windows Vista e Windows 8 sem a atualização 8.1.

O suporte técnico a um sistema antigo não significa que o sistema seja seguro ou ainda mantido pelo fabricante. O CertRadar deverá deixar essa distinção visível ao usuário.

## Fluxo planejado

```text
Detectar ambiente
       ↓
Localizar arquivos e certificados instalados
       ↓
Executar verificações compatíveis com o sistema
       ↓
Explicar a causa provável
       ↓
Oferecer somente ações seguras e autorizadas
       ↓
Testar novamente
       ↓
Gerar resumo do atendimento
```

## Busca de certificados A1

A busca deverá começar pelas pastas com maior probabilidade de conter o arquivo:

1. Downloads;
2. Área de Trabalho;
3. Documentos;
4. restante do perfil atual;
5. demais perfis acessíveis;
6. demais pastas dos discos locais;
7. unidades removíveis conectadas.

Se o arquivo não for encontrado nas pastas prioritárias, a busca continuará progressivamente por todas as pastas locais acessíveis, sem tentar contornar permissões do Windows. A especificação completa está em [docs/06_BUSCA_DE_CERTIFICADOS.md](docs/06_BUSCA_DE_CERTIFICADOS.md).

## Princípios obrigatórios

- Leitura e diagnóstico por padrão; alterações somente com autorização explícita.
- O programa nunca deverá transmitir, copiar para o suporte ou tomar posse de uma chave privada.
- Senhas de PFX e PINs não poderão ser registrados, enviados ou armazenados.
- A interface deverá permanecer responsiva e permitir cancelamento.
- Falhas de acesso, arquivos malformados e dispositivos defeituosos não poderão derrubar a aplicação.
- Toda correção deverá ser específica, registrada e seguida de novo teste.
- O desenvolvimento será orientado a testes.

## Documentação

- [Índice da documentação](docs/README.md)
- [Visão do produto](docs/01_VISAO_DO_PRODUTO.md)
- [Escopo e requisitos](docs/02_ESCOPO_E_REQUISITOS.md)
- [Arquitetura proposta](docs/03_ARQUITETURA.md)
- [Segurança e privacidade](docs/04_SEGURANCA_E_PRIVACIDADE.md)
- [Estratégia de testes](docs/05_ESTRATEGIA_DE_TESTES.md)
- [Busca de certificados](docs/06_BUSCA_DE_CERTIFICADOS.md)
- [Fluxo de suporte](docs/07_FLUXO_DE_SUPORTE.md)
- [Roadmap](docs/08_ROADMAP.md)
- [Decisões e dúvidas para revisão](docs/09_DECISOES_PARA_REVISAO.md)
- [Fluxo de implementação acompanhada](docs/10_FLUXO_DE_IMPLEMENTACAO_ACOMPANHADA.md)
- [Contrato de fatos](docs/11_CONTRATO_DE_FATOS.md)
- [Perfil de aplicação PJe](docs/12_PERFIL_PJE.md)
- [Estado da implementação](docs/STATUS.md)
- [Glossário](docs/GLOSSARIO.md)
- [Referências oficiais](docs/REFERENCIAS.md)
- [Como contribuir](CONTRIBUTING.md)
- [Política de segurança](SECURITY.md)
- [Histórico de mudanças](CHANGELOG.md)

## Tecnologia

A arquitetura candidata separa um coletor nativo, restrito e majoritariamente de leitura, de uma interface moderna para Windows 10 e 11. A compatibilidade com Windows XP deverá ser comprovada por um experimento técnico antes de fixar linguagem, compilador e empacotamento. Consulte [docs/03_ARQUITETURA.md](docs/03_ARQUITETURA.md).

## Execução

Ainda não existe versão distribuível ou assinada do produto.
Já existe uma interface de desenvolvimento em `build\x64\Debug\certradar.exe`; ela somente inicia a busca após o usuário pressionar **Iniciar busca**. Os binários dessa pasta ainda não constituem uma release assinada.

### Executar os testes do checkpoint atual

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

A base moderna usa CMake e Visual Studio Build Tools 2022. O build legado para Windows XP será mantido isolado porque exige uma toolchain antiga e um conjunto reduzido de funcionalidades.

## Licença

A licença ainda não foi escolhida. Até que um arquivo `LICENSE` seja adicionado, não se deve presumir autorização para redistribuição.

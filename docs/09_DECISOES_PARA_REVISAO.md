# Decisões para revisão

## Decisões já confirmadas

- Nome do produto: **CertRadar**.
- Repositório: `CertRadar` / pasta local `A:/Projetos/certRadar`.
- Produto portátil e voltado ao suporte técnico.
- Foco exclusivo em Windows.
- Suporte completo: Windows 10 e 11.
- Suporte compatível: Windows 7 SP1 e 8.1.
- Modo legado: Windows XP SP3.
- Sem macOS, Linux ou Windows Server.
- Busca começa por Downloads, Área de Trabalho e Documentos.
- Busca continua por todas as pastas locais acessíveis.
- Desenvolvimento orientado a testes.
- Segurança, confiabilidade e privacidade são requisitos de produto.
- Nenhuma implementação antes da revisão desta especificação.

## Decisões técnicas dependentes de experimento

### Stack e toolchain

Proposta: coletor nativo isolado e interface moderna em C#/.NET para Windows 10/11. É necessário comprovar qual toolchain consegue produzir o modo XP sem fragilizar builds modernos.

### Forma de distribuição

Opções:

1. pasta portátil com launcher e componentes visíveis;
2. executáveis moderno e legado separados;
3. único launcher com módulos embarcados.

A primeira opção é a mais transparente; a terceira é mais conveniente, mas pode aumentar complexidade e alertas de antivírus.

### Primeiro perfil especializado

Escolher entre PJe, e-CAC, PDF ou emissão fiscal após analisar chamados reais.

### Correções da primeira versão

Definir a lista mínima. Proposta:

- reiniciar serviço de cartão;
- abrir importação oficial do Windows;
- abrir data/hora;
- direcionar ao middleware oficial;
- reiniciar uma aplicação local suportada.

### Retenção de relatórios

Proposta: manter apenas durante a sessão e permitir salvamento explícito. A política final deverá considerar o fluxo real do suporte.

### Licença

Ainda não definida. Avaliar se o núcleo será aberto, se haverá distribuição comercial e quais componentes de terceiros serão usados.

### Código assinado

Necessário para distribuição real. Definir responsável, certificado de assinatura, timestamp e processo de release.

## Itens explicitamente adiados

- varredura de arquivos ZIP/RAR/7z;
- telemetria;
- painel em nuvem;
- controle remoto;
- integração com CRM;
- atualizador automático;
- exclusão de certificados ou drivers;
- suporte a rede e compartilhamentos.

## Perguntas para a revisão do proprietário

1. O primeiro perfil especializado deverá ser PJe, e-CAC ou outro?
2. A ferramenta será inicialmente interna para uma equipe ou pública no GitHub?
3. O relatório deverá desaparecer ao fechar ou permanecer até o usuário excluir?
4. O projeto pretende ser gratuito/código aberto, comercial ou ainda não decidido?
5. O modo XP deve apenas diagnosticar ou também localizar arquivos em todo o disco?

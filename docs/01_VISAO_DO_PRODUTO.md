# Visão do produto

## Problema

Atendimentos de certificado digital frequentemente começam com uma mensagem vaga, como “o certificado não aparece” ou “não consigo assinar”. O atendente precisa descobrir manualmente a versão do Windows, o local do certificado, a existência da chave privada, o estado do token, o middleware instalado, a validade, a cadeia de confiança e a aplicação em que o usuário tentou operar.

Esse processo é repetitivo, depende da experiência individual e muitas vezes termina em uma sessão de acesso remoto mais longa do que o necessário.

## Produto

O CertRadar será uma ferramenta portátil para auxiliar o suporte a certificados digitais em computadores Windows. Ela deverá automatizar a triagem, explicar os resultados em linguagem acessível e permitir correções estritamente controladas.

## Usuários

### Atendente de suporte

Precisa identificar a causa com rapidez, seguir um processo consistente e documentar o que foi testado.

### Cliente atendido

Precisa executar uma ferramenta confiável, compreender o resultado e manter controle sobre suas credenciais e sobre qualquer alteração na máquina.

### Responsável técnico da empresa

Precisa reduzir tempo médio de atendimento, erros operacionais, exposição de dados e dependência de conhecimento informal.

## Trabalho principal a realizar

> Quando um certificado digital não funciona, quero descobrir automaticamente em qual camada está a falha, para que eu execute somente a ação necessária e comprove o resultado.

## Diferencial

O diferencial não será inventar um novo formato de certificado nem substituir ferramentas oficiais. Será reunir em um fluxo único:

1. detecção do ambiente;
2. busca ordenada;
3. diagnóstico baseado em evidências;
4. correção segura e autorizada;
5. reteste;
6. resumo do atendimento.

## Objetivos

- Reduzir o tempo gasto em verificações manuais.
- Padronizar diagnósticos entre atendentes.
- Evitar tentativas aleatórias de reinstalação.
- Resolver casos simples sem acesso remoto completo.
- Indicar claramente quando o sistema operacional é a limitação.
- Proteger chaves privadas, senhas, PINs e dados pessoais.
- Transformar resoluções recorrentes em regras testáveis.

## Não objetivos

- Emitir, renovar ou revogar certificados diretamente.
- Substituir Autoridades Certificadoras, Autoridades de Registro ou o ITI.
- Recuperar matematicamente uma chave privada perdida.
- Quebrar ou descobrir senhas de PFX e PINs.
- Fazer controle remoto irrestrito.
- Ser antivírus, limpador de sistema ou atualizador genérico de drivers.
- Validar a veracidade do conteúdo de documentos assinados.
- Oferecer suporte a macOS, Linux ou Windows Server.

## Indicadores de sucesso

As metas numéricas serão definidas após observar atendimentos reais. Os indicadores iniciais são:

- percentual de chamados diagnosticados sem AnyDesk;
- tempo até a identificação da causa;
- percentual resolvido com ação guiada;
- taxa de falso diagnóstico;
- taxa de falhas ou travamentos da ferramenta;
- quantidade de dados sensíveis coletados ou transmitidos;
- percentual de relatórios aceitos sem edição manual pelo atendente.

O objetivo permanente para captura ou transmissão de chaves privadas, senhas e PINs é **zero**.

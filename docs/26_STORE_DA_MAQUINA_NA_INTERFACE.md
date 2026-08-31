# Store Pessoal da máquina na interface

## Objetivo

A T051 ajuda a detectar uma causa comum de suporte: o certificado existe, mas foi instalado em um escopo diferente do esperado pela aplicação. A janela passa a oferecer duas consultas explícitas:

- **Cert. usuário** abre `CurrentUser\MY`;
- **Cert. máquina** abre `LocalMachine\MY`.

Os resultados nunca são unidos silenciosamente. A lista, o status e o resumo copiável sempre correspondem a apenas um escopo identificado.

## Segurança

A consulta da máquina usa as mesmas garantias da enumeração já implementada no núcleo:

- `CERT_STORE_READONLY_FLAG`;
- nenhuma solicitação de elevação ou UAC;
- nenhuma tentativa de contornar acesso negado;
- nenhuma importação, remoção ou alteração;
- nenhuma aquisição de chave privada, senha ou PIN;
- execução em thread de trabalho para preservar a responsividade.

Se o Windows negar acesso, o resultado mantém `StoreScope::local_machine`, informa a indisponibilidade e preserva o código de erro local. A ausência de evidência não é apresentada como store vazio.

## Contrato de escopo

`CertificateStoreResult` agora carrega o escopo solicitado independentemente da quantidade de certificados. Isso é necessário porque não existe um primeiro `CertificateRecord` do qual inferir o store quando a coleção está vazia.

O resumo usa esse campo para escrever exatamente uma destas linhas:

```text
Store Pessoal do usuário: acessível
Store Pessoal da máquina: acessível
```

## Interface

Os botões foram compactados para manter a janela inicial em 800 pixels de largura. Durante qualquer busca ou consulta de store, ambos os botões de certificados ficam desabilitados, impedindo duas enumerações concorrentes de disputar o mesmo resultado em memória.

## Prova automatizada

Os testes exigem que a enumeração `LocalMachine` preserve o escopo mesmo sem certificados e que o resumo da máquina não contenha a expressão “Store Pessoal do usuário”.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

# Resumo sanitizado da busca

## Objetivo

A T044 transforma o resultado da busca A1 em um texto curto que o atendente pode colar no chamado. O resumo informa se a busca terminou, foi cancelada ou falhou, além das quantidades de candidatos, classificações, pastas verificadas, acessos negados e erros isolados. Desde a T047, ele também inclui os fatos de plataforma já detectados na inicialização.

## Fronteira de privacidade

O resumo é produzido a partir das contagens do `SearchResult`. Ele não percorre nem inclui o campo `path` dos candidatos. Assim, o texto compartilhável não contém:

- nome do arquivo;
- caminho completo;
- nome do perfil do Windows;
- conteúdo do PFX/P12;
- senha, PIN, certificado ou chave privada.

Os caminhos continuam visíveis apenas na lista local, porque são necessários para o cliente recuperar o arquivo encontrado. O botão **Copiar resumo** fica desabilitado durante a busca e é habilitado somente após a consolidação do resultado.

## Fluxo na interface

1. O usuário inicia a busca explicitamente.
2. A interface desabilita qualquer resumo anterior.
3. A busca termina ou preserva resultados parciais após cancelamento.
4. O usuário pressiona **Copiar resumo**.
5. O CertRadar grava texto Unicode na área de transferência do Windows e informa o resultado da operação.

Não existe envio por rede, gravação automática em disco nem compartilhamento silencioso.

## Teste de aceitação

O teste constrói um resultado com caminhos sentinela contendo nome, identificador e palavra semelhante a segredo. A saída deve conter as contagens corretas e não pode conter nenhum desses nomes ou caminhos.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

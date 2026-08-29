# Contrato de fatos

O coletor e o motor de diagnóstico se comunicam por um documento JSON local. A versão atual é `1` e qualquer versão desconhecida é recusada explicitamente.

## Campos estáveis da versão 1

- `schemaVersion`: versão inteira do contrato;
- `platform`: família, versão, build, arquitetura, elevação e modo de suporte;
- `scan`: estado, fase e contadores não sensíveis da busca;
- `certificates`: lista reservada para os fatos de certificados;
- `smartCards`: lista reservada para leitores e tokens;
- `errors`: códigos e mensagens sanitizadas de falhas recuperáveis.

O contrato não contém PIN, senha, chave privada, conteúdo de arquivos ou caminho completo por padrão. Novos campos opcionais podem ser adicionados sem alterar a versão; mudanças incompatíveis exigem uma nova versão e migração testada.

## Prova automatizada

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
```

Os testes verificam serialização, desserialização e recusa de uma versão desconhecida.

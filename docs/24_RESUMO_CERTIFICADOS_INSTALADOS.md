# Resumo sanitizado dos certificados instalados

## Objetivo

A T049 transforma a consulta visual da T048 em uma evidência curta para o chamado. Depois que a consulta de certificados termina com sucesso, **Copiar resumo** passa a copiar um texto específico para o store exibido. A T051 posterior estendeu o mesmo contrato ao store Pessoal da máquina.

O atendente consegue registrar quantos certificados existem e quais classes de problema aparecem, sem transcrever dados pessoais nem enviar o certificado.

## Conteúdo permitido

O resumo contém:

- ambiente detectado, quando disponível;
- disponibilidade e escopo do store Pessoal consultado;
- quantidade total de certificados públicos;
- contagens de ainda não válidos, válidos, próximos do vencimento e expirados;
- contagens com e sem associação aparente de chave privada;
- contagens de providers classificados como CSP, KSP ou desconhecidos;
- declaração explícita de privacidade e de que nenhuma chave foi acessada.

## Conteúdo proibido

O formatador não lê nem concatena:

- titular, nome ou CPF;
- emissor;
- número de série;
- impressão digital completa ou parcial;
- nome completo do provider;
- datas individuais;
- certificado codificado, senha, PIN ou chave privada.

O resumo trabalha apenas com enums, booleanos e quantidades. Mesmo que os registros locais contenham sentinelas sensíveis, elas não atravessam essa fronteira.

## Integração com a interface

O botão de cópia consulta a visualização ativa:

- na busca A1, mantém o resumo de arquivos da T044/T047;
- em **Cert. usuário** ou **Cert. máquina**, usa o resumo agregado desta tarefa e identifica o escopo.

O botão só é habilitado para certificados depois que a enumeração foi concluída e o store foi aberto. Uma nova operação o desabilita até haver resultado consistente.

## Prova automatizada

O teste constrói registros com nome, CPF, emissor, série, impressões digitais e provider sentinela. Ele exige as contagens e o ambiente, e exige que nenhum identificador apareça na saída.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

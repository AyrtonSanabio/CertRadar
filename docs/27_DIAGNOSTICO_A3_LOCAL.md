# Diagnóstico A3 local na interface

## Objetivo

A T052 leva à janela a primeira parte do diagnóstico de token/cartão A3. O botão **A3 local** responde, sem controle remoto manual, a três perguntas iniciais:

1. o serviço de cartão inteligente está em execução?
2. o Windows consegue enumerar leitores?
3. algum leitor informa cartão ou token presente?

O resultado aparece na lista e pode ser copiado pelo botão **Resumo**.

A T053 posterior acrescentou ao mesmo resultado uma próxima ação derivada somente desses fatos locais.

## Snapshot local

`inspect_a3_locally` cria um snapshot com:

- estado observável do serviço `SCardSvr`;
- indicação de que a consulta de leitores foi ou não executada;
- sucesso ou código de erro da WinSCard;
- leitores retornados apenas em memória local.

Se o serviço não estiver em execução, a função não chama `SCardEstablishContext`. O resultado declara “leitores não consultados” em vez de transformar falta de evidência em “nenhum leitor”.

## Privacidade e segurança

O resumo contém somente:

- estado traduzido do serviço;
- quantidade de leitores detectados;
- quantidade de cartões/tokens presentes;
- quantidade de leitores indisponíveis;
- código técnico quando a consulta WinSCard falha;
- ambiente do Windows já detectado.

Nomes de leitores não entram na interface nem na área de transferência. A operação usa timeout zero, não abre cartão, não inicia transação, não acessa chave privada, não solicita PIN e não altera o serviço.

## Limites do resultado

Esta tarefa não verifica middleware, provider, associação de chave ou assinatura. Portanto, um cartão presente significa apenas que o subsistema WinSCard reportou presença física; não significa que o certificado está pronto para PJe ou outra aplicação.

A ação autorizada para iniciar `SCardSvr`, implementada anteriormente no núcleo, não é executada automaticamente por este diagnóstico.

## Concorrência

Busca A1, enumeração de certificados e diagnóstico A3 usam threads separadas, mas a interface permite apenas uma dessas operações por vez. Isso mantém cada resultado e cada resumo associados ao comando que o usuário acionou.

## Prova automatizada

Um teste de integração local exige que leitores só sejam consultados quando o serviço estiver em execução. Um teste sintético injeta nomes sensíveis de leitores e exige apenas as contagens, provando que esses nomes não atravessam a fronteira do resumo.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

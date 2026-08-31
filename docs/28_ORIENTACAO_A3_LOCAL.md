# Orientação A3 por evidência local

## Objetivo

A T053 transforma o snapshot da T052 em uma orientação curta para o atendente, sem misturar camadas do diagnóstico. A regra é pura: recebe fatos já coletados e devolve uma das cinco fronteiras observáveis.

## Estados de triagem

| Estado | Evidência | Próxima ação apresentada |
|---|---|---|
| Serviço indisponível | `SCardSvr` não está em execução | Verificar o serviço e iniciá-lo somente com autorização |
| Consulta falhou | Serviço ativo, mas WinSCard não respondeu com sucesso | Registrar o código e verificar subsistema ou driver |
| Leitor ausente | Consulta aprovada, lista vazia | Verificar conexão, porta USB e instalação do leitor |
| Dispositivo ausente | Existe leitor, mas nenhum cartão/token presente e disponível | Reconectar o dispositivo e testar outra porta sem insistir em PIN |
| Dispositivo detectado | Pelo menos um leitor disponível reporta presença | Prosseguir com middleware, certificado e chave |

## Limite de confiança

O último estado não se chama “pronto”. A presença física não comprova:

- middleware correto;
- provider CSP/KSP utilizável;
- certificado associado ao dispositivo;
- chave privada acessível;
- PIN correto;
- assinatura funcional;
- compatibilidade com PJe ou outra aplicação.

Por isso, o texto afirma explicitamente que presença não comprova funcionamento.

## Segurança operacional

Esta tarefa apenas classifica e formata. Ela não executa a ação sugerida, não inicia serviços, não abre dispositivo, não acessa rede e não solicita consentimento ou PIN.

## Prova automatizada

O teste percorre os cinco estados em sequência, alterando um fato por vez. O teste do resumo exige a orientação conservadora para dispositivo detectado e continua garantindo a ausência dos nomes de leitores.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

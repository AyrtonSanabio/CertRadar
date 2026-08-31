# Ambiente visível na interface

## Objetivo

A T046 apresenta na janela as informações que o atendente normalmente perguntaria no começo do suporte:

- versão reconhecível do Windows;
- número do build;
- arquitetura nativa;
- nível de suporte do CertRadar;
- execução como usuário comum ou administrador.

O rótulo é independente do status da busca. Mensagens de progresso, cancelamento e conclusão não apagam o ambiente detectado.

## Tradução operacional

O núcleo já coleta números reais pela API nativa. A camada de interface os traduz sem alterar a regra de compatibilidade:

| Fato detectado | Texto apresentado |
|---|---|
| `10.0`, build menor que 22000 | Windows 10 |
| `10.0`, build 22000 ou superior | Windows 11 |
| `6.3` | Windows 8.1 |
| `6.1` com SP1 | Windows 7 SP1 |
| `5.1` com SP3 | Windows XP SP3 |
| modo `full` | suporte completo |
| modo `compatible` | suporte compatível |
| modo `legacy` | modo legado |
| demais combinações | não suportado |

A edição permanece no contrato técnico como código de produto, mas não é exibida como nome comercial enquanto não existir um mapeamento completo e testado.

## Falha segura

Se a consulta de versão lançar uma falha, a janela informa que não conseguiu detectar o Windows e continua permitindo a busca manual. O detector não solicita elevação e o rótulo não é enviado pela rede nem persistido automaticamente.

## Prova automatizada

O teste do formatador usa fatos sintéticos para Windows 10, Windows 11 ARM64 elevado, Windows 7 SP1 e Windows XP SP3. Os testes de plataforma continuam exercitando a API real do host.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

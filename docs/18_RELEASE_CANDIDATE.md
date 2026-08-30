# Gate de release candidate

## Por que o gate pode bloquear

O CertRadar não deve ser distribuído como release apenas porque compila. O gate T043 exige simultaneamente:

- pacotes Release x64 e x86 íntegros e correspondentes ao commit atual;
- evidências qualificadas de Windows 10 e Windows 11;
- assinatura A3 real em hardware de laboratório, sem exportação da chave;
- arquivo de licença escolhido e versionado;
- assinatura Authenticode válida dos binários;
- árvore de trabalho limpa.

## Executar

```powershell
& '.\scripts\check-release-readiness.ps1'
```

O script é somente leitura em relação ao projeto (exceto o JSON local de evidência), não acessa a rede e sempre lista todos os bloqueios. `ready` significa que os portões foram comprovados; `blocked` é o resultado esperado enquanto qualquer evidência externa estiver ausente.

## Estado observado em 30/08/2026

O gate permanece bloqueado por falta de VMs qualificadas, hardware A3 de laboratório, assinatura de código e licença definida. Os pacotes atuais são builds de desenvolvimento explicitamente `unsigned`; isso é intencional e não deve ser contornado por configuração local.

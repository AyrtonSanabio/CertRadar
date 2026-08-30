# Auditoria de privacidade

## Resultado

O checkpoint T042 introduziu uma barreira de sanitização imediatamente antes das duas saídas de suporte: JSON e resumo copiável. Mesmo que um `Finding` seja construído incorretamente com dados pessoais, a saída remove:

- caminhos absolutos Windows;
- tokens com nove ou mais dígitos, incluindo CPF/CNPJ formatado ou não;
- valores inline ou subsequentes a `PIN`, `senha` e `password`.

Um teste adversarial injeta CPF, caminho contendo nome de usuário, PIN e senha nos campos do diagnóstico e confirma que nenhum sentinela aparece nas saídas. Mensagens técnicas ordinárias continuam legíveis.

## Auditoria automatizada

```powershell
& '.\scripts\privacy-audit.ps1'
```

O script:

1. executa os 58 testes em x64 e Win32;
2. recusa PFX/P12, chaves e formatos criptográficos sensíveis versionados;
3. procura APIs de exportação de chave privada no código de produção;
4. procura APIs de upload/telemetria;
5. procura execução arbitrária de processos;
6. confirma ausência de gravação persistente pelo núcleo moderno;
7. gera `artifacts/privacy-audit/privacy-audit.json` sem nome de usuário ou máquina.

## Limites

A sanitização textual é uma defesa adicional, não um detector universal de dados pessoais. Nomes livres sem contexto e identificadores alfanuméricos podem não ser reconhecidos. Por isso o desenho principal continua sendo minimização: fatos internos usam mensagens controladas, caminhos passam por campos próprios e nenhum PIN, senha ou chave privada deve entrar no modelo.

A auditoria verifica o código e os testes atuais. Qualquer nova persistência, integração de chamados, telemetria ou envio de relatório exige nova análise de ameaça e consentimento explícito.

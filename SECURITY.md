# Política de segurança

## Estado

O CertRadar ainda não possui versão pública executável. Esta política será ampliada antes da primeira distribuição.

## Como relatar uma vulnerabilidade

Use um **GitHub Security Advisory privado** no repositório quando esse recurso estiver habilitado. Não publique detalhes exploráveis em uma issue aberta antes da correção.

Nunca anexe ao relato:

- certificado real `.pfx` ou `.p12`;
- chave privada;
- senha, PIN ou PUK;
- documento de cliente;
- relatório sem anonimização;
- CPF ou CNPJ completo.

Use fixtures sintéticas e passos mínimos de reprodução.

## Escopo de alta severidade

Serão tratados como potencialmente críticos:

- exfiltração ou cópia indevida de chave privada;
- captura de senha/PIN;
- execução de arquivo encontrado na busca;
- elevação de privilégio não autorizada;
- instalação silenciosa de raiz ou driver;
- atualização remota sem verificação;
- relatório contendo segredo ou dado pessoal não necessário;
- travessia de links que saia do escopo autorizado.

## Versões suportadas

Nenhuma versão executável foi lançada. A futura política de suporte seguirá a matriz documentada e indicará claramente sistemas antigos fora de manutenção do fabricante.

# Arquitetura proposta

## Situação da decisão

Esta arquitetura é uma proposta para revisão. A compatibilidade com Windows XP e com middlewares reais deverá ser comprovada em experimentos antes de a stack ser congelada.

## Diretrizes

- Isolar código legado do restante da aplicação.
- Manter diagnóstico em leitura sempre que possível.
- Executar ações corretivas em componente separado e com lista permitida.
- Não duplicar regras de diagnóstico entre versões do Windows.
- Permitir testes sem depender de um token ou certificado real.
- Fazer processos auxiliares falharem sem derrubar a interface.

## Componentes candidatos

```text
CertRadar Launcher
        │ detecta o Windows
        ├───────────────────────────┐
        ↓                           ↓
Interface moderna             Interface legada
Windows 10/11                 Windows 7/8.1/XP
        │                           │
        └───────────┬───────────────┘
                    ↓
           Coletor nativo isolado
           arquivos, stores, tokens
                    ↓
             fatos em JSON local
                    ↓
             Motor de diagnóstico
                    ↓
        conclusão, ações e relatório
                    ↓
        Executor de ações autorizadas
```

### Launcher

Responsabilidade mínima: detectar sistema e arquitetura, verificar integridade dos componentes e iniciar o módulo apropriado. Não deverá solicitar elevação nem manipular certificados.

### Coletor

Processo separado, preferencialmente nativo e x86 para maximizar compatibilidade. Deverá:

- enumerar arquivos e repositórios;
- consultar CryptoAPI/CNG/WinSCard conforme disponibilidade;
- não alterar o ambiente;
- produzir fatos estruturados;
- aplicar limites de tempo, memória e tamanho;
- encerrar de forma controlada quando cancelado.

Separá-lo em processo reduz o impacto de providers antigos ou defeituosos sobre a interface.

### Interfaces

- **Moderna:** candidata a C#/.NET com WPF para Windows 10 e 11.
- **Legada:** interface reduzida, nativa ou baseada em recursos comprovadamente disponíveis, sem depender de runtime a ser instalado no atendimento.

### Motor de diagnóstico

Recebe fatos, não acesso direto às chaves. As regras deverão ser versionadas e testáveis:

```text
Fato: certificado presente
Fato: chave privada ausente
Fato: arquivo localizado é .cer, não .pfx

Conclusão: foi instalado apenas o certificado público
Confiança: alta
Ação: localizar PFX/P12 original ou reemitir se a chave foi perdida
```

### Executor de ações

Componente separado, inativo por padrão. Cada ação terá:

- identificador estável;
- pré-condições;
- versões de Windows permitidas;
- necessidade ou não de elevação;
- descrição para consentimento;
- execução sem shell construído dinamicamente;
- verificação posterior;
- reversão quando possível.

## Contrato de fatos

O coletor deverá retornar um documento versionado, por exemplo:

```json
{
  "schemaVersion": 1,
  "platform": {
    "family": "windows",
    "mode": "full"
  },
  "scan": {
    "status": "completed",
    "candidates": 0,
    "accessDeniedCount": 0
  },
  "certificates": [],
  "smartCards": [],
  "errors": []
}
```

O exemplo não define o contrato final e propositalmente não inclui CPF, PIN, senha ou conteúdo de arquivos.

## Armazenamento

Primeira versão sem banco de dados e sem serviço em nuvem. Estado temporário e relatórios ficarão em pasta controlada dentro do perfil do usuário, com remoção definida pela política de retenção.

## Rede

A busca local não exigirá internet. Validação de revogação, download oficial de componentes e perfis de aplicações poderão exigir rede e deverão indicar claramente:

- endpoint consultado;
- finalidade;
- resultado;
- timeout;
- diferença entre “certificado inválido” e “não foi possível consultar”.

## Distribuição

O objetivo operacional é uma pasta portátil que possa ser transferida ao cliente e removida após o atendimento. A ideia de um único executável autoextraível somente será adotada se não prejudicar transparência, antivírus, assinatura de código e recuperação de falhas.

## Experimentos obrigatórios antes da implementação principal

1. Executável mínimo em XP SP3, Windows 7 SP1, 8.1, 10 e 11.
2. Enumeração de stores e associação com chave privada sem prompt.
3. Enumeração WinSCard sem consumir tentativa de PIN.
4. Isolamento de provider que trava ou encerra processo.
5. Cancelamento de busca em árvore grande e com junções.
6. Validação do contrato JSON entre coletor e interface.
7. Avaliação de assinatura de código e comportamento do SmartScreen/antivírus.

Sem esses resultados, linguagem, toolchain e empacotamento permanecem decisões abertas.

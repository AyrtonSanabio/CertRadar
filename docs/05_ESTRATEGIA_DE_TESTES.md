# Estratégia de testes

## Método

O desenvolvimento será orientado a testes:

```text
1. Escrever um teste que representa o requisito
2. Confirmar que ele falha pelo motivo esperado
3. Implementar o mínimo necessário
4. Confirmar que o teste passa
5. Refatorar sem alterar o comportamento
```

Correções de bugs deverão começar por um teste de regressão que reproduza a falha.

## Pirâmide de testes

### Unitários

- ordenação da fila de busca;
- normalização e mascaramento;
- classificação de validade;
- regras de diagnóstico;
- política de ações permitidas;
- serialização e migração do contrato de fatos;
- cancelamento e timeouts simulados.

### Baseados em propriedades e fuzzing

- caminhos Unicode, longos e malformados;
- documentos JSON truncados ou desconhecidos;
- contêineres PFX/P12 malformados;
- extensões com variação de caixa;
- árvore com ciclos, junções e arquivos mutáveis;
- regras que nunca podem produzir ação destrutiva sem consentimento.

### Integração

- stores Current User e Local Machine;
- certificado público sem chave;
- certificado com chave exportável e não exportável;
- cadeia completa, incompleta, vencida e revogada;
- serviço de cartão parado;
- leitor sem cartão;
- provider ausente ou com falha;
- busca em disco removível;
- pasta com acesso negado.

### Ponta a ponta

- iniciar, diagnosticar, autorizar ação, retestar e gerar relatório;
- cancelar busca completa;
- fechar durante operação e recuperar sem deixar segredo temporário;
- executar no modo correto para cada Windows.

## Matriz de plataformas

| Sistema | Diagnóstico | Busca completa | Teste de assinatura | Correções | Execução em VM obrigatória |
|---|---:|---:|---:|---:|---:|
| Windows 11 x64 | Sim | Sim | Sim | Sim | Sim |
| Windows 10 x64 | Sim | Sim | Sim | Sim | Sim |
| Windows 10 x86 | Quando disponível | Sim | Sim | Sim | Sim |
| Windows 8.1 x86/x64 | Sim | Sim | Mediante prova | Limitadas | Sim |
| Windows 7 SP1 x86/x64 | Sim | Sim | Mediante prova | Limitadas | Sim |
| Windows XP SP3 x86 | Básico | Sim | Somente se seguro | Não inicialmente | Sim |

Resultados em uma versão não poderão ser extrapolados para outra sem teste.

## Casos obrigatórios da busca

- Downloads é visitado antes de outras pastas.
- Área de Trabalho e Documentos mantêm prioridade.
- Busca continua depois de pasta sem permissão.
- Junção para diretório pai não cria loop.
- Dois caminhos para o mesmo arquivo não geram dois resultados.
- Arquivo desaparece durante leitura sem derrubar a aplicação.
- Unidade removível é desconectada durante a busca.
- Cancelamento interrompe novas leituras e encerra workers.
- Interface continua respondendo com milhões de entradas simuladas.
- Arquivo enorme é recusado pelo limite sem ser carregado por inteiro.
- Arquivo com extensão correta mas conteúdo inválido é classificado como suspeito, não executado.

## Certificados de teste

- Usar uma autoridade certificadora exclusivamente de laboratório.
- Gerar chaves e certificados sintéticos durante os testes sempre que possível.
- Nunca colocar certificados reais de clientes no repositório.
- Nunca versionar senha real.
- Fixtures maliciosas deverão ser sintéticas e documentadas.

## Qualidade e cobertura

Cobertura não substitui bons testes, mas servirá como alarme:

- mínimo planejado de 90% de branches para motor de regras, mascaramento e política de ações;
- mínimo planejado de 80% de branches para o núcleo restante testável;
- nenhum limite de cobertura justificará testar uma chave ou token real de cliente.

## Portões de entrega

Uma entrega não poderá ser considerada pronta com:

- teste falhando ou ignorado sem justificativa;
- warning novo do compilador ou análise estática;
- crash conhecido sem contenção;
- segredo em log ou fixture;
- plataforma declarada sem execução em VM;
- correção sem teste de regressão;
- comportamento não descrito nos requisitos.

## Prova para revisão

Cada marco deverá entregar:

1. comando exato de testes;
2. quantidade de testes executados, aprovados e ignorados;
3. sistemas/VMs usados;
4. limitações conhecidas;
5. amostra sanitizada do relatório;
6. diff documental correspondente.

# Busca de certificados

## Objetivo

Localizar arquivos A1 prováveis com rapidez nas pastas mais comuns e, se necessário, continuar por todas as pastas locais acessíveis de forma segura, cancelável e previsível.

## Tipos iniciais

- `.pfx`
- `.p12`

Extensões serão comparadas sem diferenciar maiúsculas e minúsculas. Arquivos `.cer`, `.crt`, `.p7b` e `.pem` poderão ser inventariados em uma fase futura, mas não serão apresentados como backup A1 sem evidência de chave privada.

Arquivos compactados não fazem parte da primeira versão porque aumentam significativamente o risco, o custo de leitura e a possibilidade de conteúdo recursivo ou malicioso.

## Ordem da busca

### Fase 1 — Pastas prioritárias

1. Downloads do usuário atual.
2. Área de Trabalho do usuário atual.
3. Documentos do usuário atual.

As pastas deverão ser obtidas pelas APIs do Windows, não por nomes ou caminhos fixos, pois podem ter sido movidas ou traduzidas.

### Fase 2 — Perfil atual

Percorrer o restante do perfil, descontando diretórios já processados.

### Fase 3 — Outros perfis acessíveis

Enumerar somente conteúdo que o usuário atual pode ler. Nenhuma elevação automática será solicitada para inspecionar outro perfil.

### Fase 4 — Discos locais

Percorrer os demais diretórios de unidades fixas. Pastas do sistema não serão excluídas apenas pelo nome; se forem acessíveis, poderão ser enumeradas. Acesso negado será contabilizado e a busca seguirá.

### Fase 5 — Unidades removíveis

Percorrer pendrives e discos removíveis conectados. Se forem removidos durante a busca, registrar a interrupção daquela unidade sem encerrar a aplicação.

Compartilhamentos, unidades remotas e caminhos UNC ficam fora da busca padrão.

## Estratégia de iteração

Usar uma fila de trabalho por prioridade. A busca deverá ser progressiva: resultados aparecem assim que encontrados, sem esperar o disco inteiro.

Para evitar ciclos e duplicidades:

- canonicalizar o caminho;
- obter identidade estável do arquivo/diretório quando a API permitir;
- manter conjunto de itens visitados;
- não atravessar pontos de nova análise;
- não seguir atalhos `.lnk`;
- não voltar para diretório pai por link;
- limitar concorrência por unidade.

## Validação do candidato

Encontrar a extensão não prova que o arquivo é um PFX válido. O processo deverá:

1. registrar metadados mínimos;
2. rejeitar leitura integral acima do limite configurado;
3. verificar de forma segura se a estrutura parece PKCS#12;
4. não pedir senha durante a busca;
5. classificar como “candidato”, “contêiner reconhecido” ou “arquivo inválido”;
6. somente exibir identidade interna após ação explícita e autenticação local, se necessária.

O limite inicial de tamanho será definido por medição. A proposta para o experimento é 32 MiB, sem tratá-la como valor final.

## Duplicidades

Dois critérios possíveis:

- mesma identidade de arquivo: deduplicação imediata;
- conteúdo possivelmente igual em arquivos distintos: hash somente sob demanda ou quando o custo for aceitável.

O hash de um PFX é dado técnico sensível e não deverá sair da máquina no relatório padrão.

## Cancelamento

Ao cancelar:

- parar de enfileirar diretórios;
- cancelar leituras pendentes quando possível;
- aguardar workers com prazo curto;
- liberar handles;
- preservar resultados já encontrados;
- marcar a busca como parcial.

## Resultado esperado

```text
Busca parcial concluída

Pastas prioritárias verificadas: 3
Demais pastas verificadas: 18.420
Pastas sem acesso: 27
Arquivos candidatos: 2
Contêineres reconhecidos: 1

Nenhum arquivo foi enviado ou alterado.
```

## Restrições de segurança

- Não executar arquivo encontrado.
- Não abrir com aplicativo associado.
- Não copiar para pasta do CertRadar.
- Não alterar permissões.
- Não assumir propriedade.
- Não montar imagem ou arquivo compactado.
- Não testar senha.
- Não calcular hash de todos os arquivos do computador.

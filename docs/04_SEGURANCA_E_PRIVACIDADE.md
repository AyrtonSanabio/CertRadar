# Segurança e privacidade

## Princípio central

> O CertRadar pode localizar e diagnosticar uma credencial, mas nunca deve tomar posse dela.

## Ativos protegidos

- chave privada A1;
- chave privada em token/cartão;
- senha de PFX;
- PIN e PUK;
- CPF, CNPJ, nome e identificadores do certificado;
- caminhos e nomes de arquivos;
- documentos e dados do cliente;
- integridade do repositório de confiança do Windows;
- integridade da máquina atendida.

## Ameaças consideradas

- arquivo PFX malformado criado para explorar o parser;
- árvore com junções, links ou caminhos enormes;
- arquivo removido ou alterado durante a leitura;
- pasta sem permissão;
- token, driver ou provider que trava;
- relatório compartilhado contendo dados pessoais;
- atendente ou atacante tentando obter o PFX;
- instalação de raiz ou driver de origem falsa;
- execução com privilégios administrativos desnecessários;
- atualização adulterada;
- reaproveitamento indevido de dados temporários;
- logs contendo PIN, senha, CPF ou chave.

## Controles obrigatórios

### Privilégio mínimo

O diagnóstico básico deverá funcionar como usuário comum. Elevação deverá ocorrer somente para uma ação específica, após consentimento, e terminar junto com a ação.

### Leitura limitada

Durante a localização, o programa deverá ler metadados e somente a quantidade mínima necessária para reconhecer um contêiner candidato. Não deverá carregar arquivos arbitrariamente grandes em memória.

### Segredos

- Não registrar senha, PIN ou PUK.
- Não oferecer campo genérico de senha se for possível usar a interface segura do sistema/provider.
- Não copiar PFX para pasta temporária.
- Não anexar PFX ou P12 ao relatório.
- Não exportar chave automaticamente.
- Não testar combinações de senha.

### Sistema de arquivos

- Canonicalizar caminhos antes de processar.
- Não seguir pontos de nova análise por padrão.
- Manter conjunto de diretórios/arquivos já visitados.
- Impor limites de tamanho, profundidade operacional e tempo por item.
- Tratar nomes Unicode e caminhos longos.
- Não executar arquivos encontrados.
- Não abrir arquivos por associação de shell.

### Ações corretivas

- Lista permitida, versionada e testada.
- Consentimento explícito por ação.
- Origem oficial para downloads.
- Verificação de assinatura digital e, quando publicado, hash.
- Nenhuma instalação silenciosa de raiz, driver ou middleware.
- Nenhuma exclusão de chave/certificado na primeira versão.
- Reteste e registro do resultado.

### Relatórios

O relatório padrão poderá conter:

- versão e arquitetura do Windows;
- versão do CertRadar;
- categorias de resultado;
- datas de validade;
- emissor quando necessário;
- identificadores pseudonimizados;
- ações e retestes.

O relatório padrão não deverá conter:

- chave privada;
- senha, PIN ou PUK;
- conteúdo de documentos;
- caminho completo sem necessidade;
- CPF/CNPJ completos;
- nome completo por padrão;
- número de série completo quando não indispensável.

### Rede e telemetria

A primeira versão não terá telemetria. Qualquer consulta online deverá ser funcional, visível e necessária, como validação de revogação ou acesso a fonte oficial. Falha de rede não poderá ser apresentada automaticamente como certificado inválido.

## Busca em todas as pastas

“Todas as pastas” significa todas as pastas de unidades locais e removíveis que o usuário atual pode enumerar. Não significa:

- burlar permissões;
- assumir propriedade de pastas;
- montar volumes ocultos;
- vasculhar rede corporativa;
- seguir links para fora da unidade;
- ler conteúdo de arquivos que não sejam candidatos.

## Desenvolvimento seguro

- Revisão obrigatória para alterações de parsing, criptografia, elevação ou atualização.
- Análise estática no CI.
- Sanitizers e verificadores de memória nos componentes nativos quando suportados.
- Fuzzing do parser e do contrato de entrada.
- Dependências pinadas e inventariadas.
- Build reproduzível como objetivo antes da distribuição pública.
- Binários de distribuição assinados.

## Incidentes

Se houver indício de que o CertRadar copiou, expôs ou alterou uma chave privada, a versão afetada não deverá continuar sendo distribuída até investigação e correção. Consulte também [SECURITY.md](../SECURITY.md).

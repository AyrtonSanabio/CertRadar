# Como contribuir

O CertRadar está em fase de especificação. Antes de implementar uma funcionalidade, confirme que existe um requisito identificado em `docs/02_ESCOPO_E_REQUISITOS.md`.

Toda implementação também deverá seguir `docs/10_FLUXO_DE_IMPLEMENTACAO_ACOMPANHADA.md`: uma tarefa por vez, pausa obrigatória para estudo e avanço somente após aprovação explícita do proprietário.

## Fluxo obrigatório

1. Criar ou selecionar o requisito.
2. Descrever risco e dados tratados.
3. Escrever o teste que falha pelo motivo esperado.
4. Implementar o mínimo necessário.
5. Executar testes unitários e de integração relevantes.
6. Refatorar mantendo os testes verdes.
7. Atualizar documentação e changelog.
8. Apresentar evidência para revisão.
9. Parar e aguardar aprovação antes de iniciar outra tarefa.

## Regras de segurança

- Nunca adicionar PFX/P12, chave, senha ou PIN real ao repositório.
- Nunca adicionar relatório real de cliente sem anonimização verificável.
- Nunca executar um arquivo localizado pelo scanner.
- Nunca usar comandos de shell concatenando caminho recebido do sistema de arquivos.
- Não adicionar dependência sem registrar finalidade, licença e risco.
- Alterações de parsing, elevação, atualização, criptografia ou mascaramento exigem revisão específica.

## Testes

- Todo bug corrigido precisa de teste de regressão.
- Testes devem usar certificados sintéticos.
- Testes dependentes de sistema devem declarar a versão executada.
- Um teste ignorado precisa de justificativa visível.
- A matriz completa está em `docs/05_ESTRATEGIA_DE_TESTES.md`.

## Commits

Nenhum commit deverá ser criado enquanto houver revisão solicitada e ainda não aprovada pelo proprietário do projeto. Antes de commit, revisar `git diff`, resultados de testes e ausência de segredos.

## Definição de pronto

Consulte o critério geral em `docs/02_ESCOPO_E_REQUISITOS.md`. “Funciona na minha máquina” não é prova suficiente para uma plataforma declarada.

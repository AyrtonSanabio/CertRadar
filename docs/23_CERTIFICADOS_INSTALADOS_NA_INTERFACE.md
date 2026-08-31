# Certificados instalados na interface

## Objetivo

A T048 torna visível na janela uma capacidade que já existia no núcleo: enumerar os certificados do store pessoal do usuário atual. O atendente pode pressionar **Instalados** e verificar rapidamente se há um certificado público disponível, se ele está válido e se existe associação aparente com uma chave privada.

## Fluxo

1. O usuário pressiona **Instalados**.
2. A interface desabilita temporariamente as ações incompatíveis e inicia uma thread de trabalho.
3. O núcleo abre o store `CurrentUser\MY` somente para leitura e enumera os certificados públicos.
4. A thread publica o resultado de volta para a janela.
5. A lista mostra uma linha sanitizada por certificado, ou uma mensagem clara quando o store está vazio ou inacessível.

A enumeração não acontece na thread da janela. Assim, um store lento não impede que o Windows continue redesenhando a aplicação.

## Dados apresentados

Cada linha contém somente:

- índice temporário na lista;
- oito caracteres finais da impressão digital, suficientes para diferenciar visualmente resultados próximos;
- estado de validade;
- existência aparente de associação com chave privada;
- categoria do provider (`CSP`, `KSP`, desconhecido ou ausente);
- data final de validade em UTC.

Não são mostrados titular, CPF, emissor, número de série, impressão digital completa nem nome completo do provider. Esses dados continuam disponíveis apenas nas estruturas locais que alimentam diagnósticos posteriores, não na apresentação operacional desta tarefa.

## Limites de segurança

- O store é aberto em modo somente leitura.
- Nenhuma chave privada é adquirida, exportada ou testada.
- A propriedade de chave é apenas metadado do certificado; ela não prova que a chave ou o token esteja funcional.
- Nenhum PIN, senha, elevação ou consentimento para mutação é solicitado.
- **Mostrar arquivo** permanece desabilitado nessa visualização, pois um certificado instalado não é um resultado de arquivo PFX/P12.
- Nesta tarefa, **Copiar resumo** permanecia restrito à busca de arquivos. A T049 posterior adicionou um resumo agregado e sanitizado específico para esta visualização.
- Nesta tarefa, o store `LocalMachine\MY` ainda não estava integrado à janela. A T051 posterior adicionou uma consulta separada, somente leitura e sem elevação.

## Prova automatizada

O teste usa um certificado sintético com titular, emissor, série, provider e impressão digital sentinela. Ele exige a presença dos fatos operacionais mínimos e a ausência desses identificadores completos.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

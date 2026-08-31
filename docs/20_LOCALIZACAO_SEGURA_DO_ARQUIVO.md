# Localização segura do arquivo

## Objetivo

A T045 fecha o caminho entre “o CertRadar encontrou um candidato” e “o cliente consegue vê-lo na pasta”. Depois da busca, o usuário seleciona um item da lista e pressiona **Mostrar arquivo**. O Windows Explorer abre a pasta-pai com o item destacado.

## Planejamento antes do efeito

O núcleo recebe o `SearchResult` consolidado e o índice selecionado. O plano somente fica pronto quando:

- o índice pertence à lista de candidatos;
- o caminho não está vazio;
- o caminho é absoluto;
- o caminho não contém um segmento `..`.

A interface não aceita texto digitado, argumento de linha de comando nem caminho recebido pela rede. O alvo precisa ter sido produzido pela busca local e selecionado na lista correspondente.

## Integração com o Windows

A thread da interface inicializa um apartamento COM. Para um plano aprovado, `ILCreateFromPathW` converte o caminho em um identificador do Shell e `SHOpenFolderAndSelectItems` abre a pasta-pai com o item selecionado. A memória do identificador é liberada após a chamada e o apartamento COM é encerrado de forma balanceada.

Documentação oficial: [SHOpenFolderAndSelectItems](https://learn.microsoft.com/pt-br/windows/win32/api/shlobj_core/nf-shlobj_core-shopenfolderandselectitems) e [ILCreateFromPathW](https://learn.microsoft.com/pt-br/windows/win32/api/shlobj_core/nf-shlobj_core-ilcreatefrompathw).

## Limites de segurança

- O arquivo não é aberto, importado, copiado, executado ou alterado.
- Não é usada associação de arquivo nem comando construído dinamicamente.
- O clique no botão é o consentimento explícito para abrir o Explorer.
- Se o arquivo tiver sido movido, removido ou estiver em mídia desconectada, a ação falha e informa isso na própria janela.

## Prova automatizada

O teste cobre um candidato absoluto válido, um caminho relativo, um caminho com travessia `..` e um índice inexistente.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

A captura visual automatizada não foi possível neste host porque o controlador de janelas rejeitou a janela Win32 com `SetIsBorderRequired failed (0x80004002)`. Essa limitação de ferramenta não é registrada como prova visual; build, modelo da interface e integração de link são validados pelos testes e pelo empacotamento.

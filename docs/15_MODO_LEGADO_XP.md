# Modo legado Windows XP SP3

## Estado

Existe um coletor legado separado, compilado com MinGW.org GCC 6.3 para x86/i586, PE32 e subsistema Windows 5.01. Ele foi compilado, inspecionado e executado no host Windows 10, mas ainda não foi executado em Windows XP SP3.

Por esse motivo, T040 permanece parcial e o nome do pacote inclui `legacy-untested`.

## Escopo implementado

- executável de console isolado do núcleo C++17;
- C++98 com APIs Win32 disponíveis no alvo 5.01;
- prioridade para Downloads, Área de Trabalho e Documentos quando existirem;
- varredura iterativa posterior do perfil atual;
- reconhecimento somente pelos nomes `.pfx` e `.p12`;
- reparse points ignorados;
- limite de `MAX_PATH` e máximo de 100.000 diretórios;
- nenhum conteúdo de certificado aberto;
- nenhuma senha, PIN, rede, store, token ou correção.

Pela limitação ANSI da camada legada, caminhos com caracteres fora da página de código do sistema podem não ser enumerados. Essa degradação é preferível a prometer suporte Unicode que a toolchain e o XP não conseguem comprovar.

## Compilar, verificar e empacotar

```powershell
& '.\tests\legacy_package_tests.ps1'
```

O teste compila estaticamente, interpreta a tabela de imports do PE, confirma x86/subsistema 5.01, executa uma busca controlada e valida o ZIP e os hashes.

Saída local ignorada pelo Git:

```text
artifacts/CertRadar-0.1.0-windows-xp-x86-legacy-untested/
artifacts/CertRadar-0.1.0-windows-xp-x86-legacy-untested.zip
artifacts/CertRadar-0.1.0-windows-xp-x86-legacy-untested.zip.sha256
```

## Critério restante para T040

Executar em VM Windows XP SP3 x86: inicialização, pastas prioritárias reais, perfil com acesso negado, árvore profunda, reparse point disponível e encerramento normal. Até essa evidência existir, o pacote não deve ser entregue como compatível.

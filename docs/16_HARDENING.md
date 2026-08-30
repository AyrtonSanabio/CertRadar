# Hardening: mutação e análise estática

## Campanhas determinísticas

A suíte moderna contém três testes de robustez reproduzíveis:

- 5.000 sequências arbitrárias de até 512 bytes no desserializador JSON;
- 5.000 sequências arbitrárias nos mascaradores de identificador e caminho;
- 500 blobs arbitrários de até 2 KiB na inspeção PKCS#12, sempre sob limite de 4 KiB.

A semente do gerador xorshift é fixa. Assim, uma falha pode ser repetida em x64 e Win32 sem armazenar material de certificado real.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

Essa campanha é mutacional e limitada. Ela não mede cobertura e não substitui libFuzzer, ASan ou execução contínua. O host atual possui `clang-tidy`, mas não o compilador Clang/libFuzzer; essa ampliação fica registrada como trabalho futuro.

## Análise estática

```powershell
& '.\scripts\static-analysis.ps1'
```

O script gera `compile_commands.json` com CMake/Ninja/MSVC e executa `clang-tidy` em todos os fontes de `src/`, com famílias `clang-analyzer`, `bugprone` e `performance`. Caminhos alternativos das ferramentas podem ser fornecidos por parâmetro.

Três regras são excluídas de forma estreita:

- `bugprone-easily-swappable-parameters`: APIs numéricas como início/fim/agora têm ordem documentada e testada;
- `clang-analyzer-optin.core.EnumCastOutOfRange`: falso positivo na bitmask interna do `std::filesystem` do MSVC;
- `performance-no-int-to-ptr`: a API Win32 representa IDs de controles e cores de sistema como handles numéricos.

## Correção produzida pela análise

Funções de busca em catálogos estáticos estavam declaradas `noexcept`, embora a primeira inicialização dos vetores e a normalização do provider possam alocar memória. As declarações foram corrigidas para evitar encerramento por `std::terminate` caso uma exceção de alocação ocorra. Os `switch` da janela também ganharam ramos padrão explícitos.

# Pacote portátil para Windows 10 e 11

## Objetivo

O checkpoint T037 produz uma pasta e um ZIP portáteis por arquitetura. O pacote não instala serviço, não grava no Registro e executa com o mesmo privilégio do usuário (`asInvoker`).

## Gerar e testar

```powershell
& '.\tests\package_tests.ps1' -Architecture x64
& '.\tests\package_tests.ps1' -Architecture Win32
```

Cada execução:

1. configura e compila o Release com runtime C++ estático;
2. executa a suíte CTest da arquitetura;
3. monta uma pasta a partir de uma allowlist;
4. gera e verifica `SHA256SUMS.txt`;
5. confirma o cabeçalho PE x64 ou x86;
6. confirma que o binário desta etapa não está assinado;
7. cria o ZIP e seu hash externo;
8. inicia ocultamente o processo GUI empacotado, aguarda o estado ocioso da interface e o encerra.

## Saídas locais

```text
artifacts/CertRadar-0.1.0-windows-x64-unsigned/
artifacts/CertRadar-0.1.0-windows-x64-unsigned.zip
artifacts/CertRadar-0.1.0-windows-x64-unsigned.zip.sha256
artifacts/CertRadar-0.1.0-windows-x86-unsigned/
artifacts/CertRadar-0.1.0-windows-x86-unsigned.zip
artifacts/CertRadar-0.1.0-windows-x86-unsigned.zip.sha256
```

Os artefatos são locais e ignorados pelo Git. A pasta contém somente `certradar.exe`, `LEIA-ME.txt`, `SECURITY.md` e `SHA256SUMS.txt`.

## Limitações comprovadas

- O teste de inicialização atual foi executado no host Windows 10 do desenvolvimento.
- Windows 11 será validado na matriz de VMs do T038.
- O pacote é deliberadamente identificado como `unsigned`; não é uma release pública.
- A licença do projeto ainda não foi escolhida, portanto o pacote é apenas para avaliação controlada.

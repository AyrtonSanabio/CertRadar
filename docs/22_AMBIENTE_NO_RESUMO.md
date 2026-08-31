# Ambiente no resumo do chamado

## Objetivo

A T047 elimina a digitação manual do Windows e da arquitetura no chamado. Quando a plataforma foi detectada na inicialização, **Copiar resumo** inclui a mesma linha apresentada na janela antes das contagens da busca.

Exemplo estrutural:

```text
Resumo CertRadar — busca de certificado A1
Ambiente: Windows 10 (build 19045) x64 — suporte completo — usuário comum
Busca concluída
Candidatos encontrados: 2
...
```

## Fonte única dos fatos

A aplicação detecta o ambiente uma vez e mantém o objeto `WindowsPlatform` somente em memória. A interface e o resumo usam o mesmo formatador público. Pressionar **Copiar resumo** não consulta novamente o sistema e não pode produzir divergência entre o que aparece na janela e o que vai para o chamado.

Se a detecção falhar, o objeto não é armazenado e o resumo básico continua disponível. A ausência de evidência não é convertida em uma versão presumida do Windows.

## Privacidade

Versão, build, arquitetura, nível de suporte e privilégio do processo são fatos técnicos necessários ao atendimento. O resumo continua sem incluir nome de usuário, edição comercial, nome de arquivo, caminho, certificado, identificador pessoal, senha, PIN ou chave privada.

## Prova automatizada

O teste enriquece um resultado que contém caminhos sentinela. Ele exige a presença de Windows/build/arquitetura e continua exigindo a ausência de todos os nomes e caminhos locais.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

# Matriz de compatibilidade e evidências de VM

## Estado atual

O host de desenvolvimento é Windows 10 Pro `10.0.19045` x64. Em 30 de agosto de 2026, Hyper-V estava desativado e não havia VMware nem VirtualBox disponíveis. Por isso:

- o smoke test local do Windows 10 é evidência válida apenas para inicialização;
- o fluxo completo do T038 continua pendente em Windows 10 e Windows 11 controlados;
- T039 continua pendente em Windows 7 SP1 e Windows 8.1;
- nenhum desses sistemas deve ser marcado como qualificado sem a execução na VM correspondente.

## Coleta dentro de uma VM

Copie o repositório de teste e o pacote da arquitetura apropriada para a VM e execute:

```powershell
& '.\scripts\collect-vm-evidence.ps1' `
  -PackageDirectory '.\artifacts\CertRadar-0.1.0-windows-x64-unsigned' `
  -Architecture x64
```

O coletor verifica hashes, arquitetura PE e inicialização. Depois gera JSON local com:

- versão e arquitetura do Windows;
- identificador e SHA-256 do pacote;
- verificações automatizadas aprovadas;
- checklist manual ainda pendente.

Nome da máquina, domínio e usuário não são coletados. O JSON nunca declara qualificação completa automaticamente.

## Critério para concluir T038/T039

Um responsável deverá executar e registrar os itens manuais na VM: abertura sem elevação, busca prioritária, pausa/continuação, cancelamento, busca completa e revisão do resumo sanitizado. A evidência automatizada isolada não basta.

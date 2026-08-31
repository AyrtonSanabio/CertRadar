# Prioridade visual dos certificados instalados

## Objetivo

A T050 reduz o tempo de triagem quando o store contém muitos certificados. A lista de **Instalados** passa a apresentar primeiro as situações com maior chance de exigir ação do suporte, sem apagar ou esconder os demais registros.

## Ordem aplicada

1. expirados;
2. ainda não válidos;
3. vencem em até 30 dias;
4. válidos sem associação aparente de chave privada;
5. válidos com associação aparente de chave privada.

Dentro da mesma classe, a ordem de enumeração do Windows é preservada. A ordenação é estável para evitar que itens equivalentes mudem de posição sem motivo observável.

## Separação entre regra e interface

O núcleo de apresentação recebe o resultado somente leitura e devolve um vetor de índices. Ele não copia, modifica nem reordena os objetos `CertificateRecord`. A janela percorre esses índices apenas ao construir suas linhas.

Essa separação permite testar a prioridade sem depender da GUI ou de certificados reais instalados.

## Limites

A posição é uma prioridade de triagem, não uma conclusão clínica sobre a credencial. Em especial:

- associação aparente não comprova que a chave funciona;
- um certificado válido pode ter falha de cadeia, revogação ou middleware;
- um certificado expirado não deve ser removido automaticamente;
- nenhum item é alterado no store.

## Prova automatizada

O teste cria todas as classes em ordem inversa, inclui dois expirados consecutivos e exige tanto a prioridade quanto a estabilidade.

```powershell
& '.\scripts\test.ps1' -Architecture x64 -Configuration Debug
& '.\scripts\test.ps1' -Architecture Win32 -Configuration Debug
```

# Fluxo de suporte

## Atendimento principal

### 1. Transferência

O atendente transfere a pasta portátil do CertRadar para a máquina do cliente. O binário deverá ser assinado antes de distribuição pública.

### 2. Consentimento e objetivo

O cliente vê o que será verificado e escolhe o objetivo:

- localizar certificado A1;
- verificar se o certificado funciona;
- diagnosticar token/cartão;
- preparar acesso a uma aplicação suportada.

### 3. Triagem rápida

Executar primeiro verificações de baixo custo:

- sistema e arquitetura;
- stores de certificados;
- validade e chave privada;
- leitores, cartões, serviço e providers;
- Downloads, Área de Trabalho e Documentos.

### 4. Busca ampliada

Se necessário, o usuário inicia a busca nas demais pastas. Ela deverá mostrar progresso e aceitar cancelamento.

### 5. Diagnóstico

Apresentar em três níveis:

```text
Fato observado
Causa provável e confiança
Próxima ação recomendada
```

Exemplo:

```text
Fato: certificado público encontrado, sem chave privada associada.
Causa provável: foi importado um CER ou a chave estava em outro perfil.
Confiança: alta.
Próxima ação: localizar o PFX/P12 original. Se a chave foi perdida, orientar revogação/reemissão.
```

### 6. Correção autorizada

O CertRadar poderá oferecer somente ações conhecidas e testadas, como:

- iniciar/reiniciar serviço de cartão;
- abrir o assistente oficial de importação;
- abrir configuração de data/hora;
- abrir fonte oficial do middleware identificado;
- reiniciar componente local suportado;
- instalar cadeia oficial com consentimento e validação, em fase posterior.

### 7. Reteste

Repetir verificações afetadas e mostrar comparação antes/depois.

### 8. Encerramento

Gerar resumo sanitizado com:

- objetivo;
- ambiente;
- fatos relevantes;
- conclusão;
- ações autorizadas;
- resultado do reteste;
- limitações restantes.

## Exemplo de resumo

```text
Objetivo: acesso ao PJe
Sistema: Windows 10 x64
Certificado: presente, válido e com chave privada
Token: detectado
Problema: serviço de cartão inteligente parado
Ação autorizada: serviço iniciado
Reteste: assinatura de desafio aprovada
Resultado: ambiente local funcional
```

## Fluxo sem solução automática

O programa deverá saber parar:

```text
Diagnóstico inconclusivo.

O certificado e a chave estão disponíveis, mas a aplicação de destino
continua recusando a operação. Compartilhe o relatório sanitizado e os
logs específicos da aplicação com o suporte.
```

## Limites do atendente

O atendente não deverá solicitar pelo CertRadar:

- upload de PFX;
- senha ou PIN;
- exportação de chave;
- desativação de antivírus;
- instalação de raiz não oficial;
- execução invisível;
- ação destrutiva sem outro procedimento formal.

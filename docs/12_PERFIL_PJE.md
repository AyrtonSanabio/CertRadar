# Perfil de aplicação: PJe

## Objetivo

O perfil `pje` responde se as evidências locais necessárias ao uso de certificado digital no PJe estão aparentemente prontas. Ele não acessa processos, não automatiza login e não captura PIN, senha ou conteúdo de documentos.

O PJe foi escolhido como primeiro perfil porque representa o cenário jurídico que motivou o produto. A documentação oficial descreve o PJeOffice como o componente usado no acesso ao PJe com certificado digital e na assinatura eletrônica de documentos.

## Verificações locais

O avaliador recebe fatos já coletados e mantém cada camada separada:

1. presença do certificado;
2. período de validade;
3. associação observável com chave privada;
4. confiança da cadeia local;
5. presença do PJeOffice;
6. execução do PJeOffice;
7. estado do dispositivo A3, somente quando o certificado depende de dispositivo externo.

O resultado geral pode ser `ready`, `attention`, `blocked` ou `indeterminate`. Evidência ausente nunca é tratada como sucesso.

## Limites

- `ready` significa apenas que as verificações locais conhecidas passaram; não garante disponibilidade do tribunal ou sucesso do login.
- O perfil não fixa navegador, versão do PJeOffice ou configuração particular de tribunal.
- O perfil não afirma que todo ambiente PJe aceita exatamente os mesmos tipos de certificado.
- Problemas de serviço, leitor, dispositivo, middleware e chave A3 permanecem separados.
- Instalação e atualização do PJeOffice continuam manuais e devem partir de fonte oficial.

## Referências oficiais

- [PJeOffice Pro — Documentação PJe](https://docs.pje.jus.br/servicos-negociais/pjeoffice-pro/)
- [Certificação Digital — Portal CNJ](https://www.cnj.jus.br/programas-e-acoes/processo-judicial-eletronico-pje/certificacao-digital-2/)

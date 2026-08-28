# Glossário

## A1

Certificado cujo material de chave costuma ser armazenado em software, frequentemente em contêiner PKCS#12 com extensão `.pfx` ou `.p12`.

## A3

Certificado cuja chave privada é protegida por dispositivo criptográfico, como token/cartão, ou por serviço remoto em HSM, conforme a modalidade.

## AC

Autoridade Certificadora. Emite e assina certificados segundo uma política.

## AR

Autoridade de Registro. Identifica e cadastra o solicitante, encaminhando a solicitação à AC.

## Cadeia de confiança

Sequência do certificado do titular, certificados intermediários e raiz confiável usada para verificar a emissão.

## Certificado digital

Documento eletrônico assinado por uma autoridade que associa uma identidade a uma chave pública e a regras de uso. O certificado público, isoladamente, não contém a chave privada.

## Chave privada

Segredo usado para assinar ou realizar outras operações criptográficas. Sua perda ou exposição pode exigir revogação e nova emissão.

## Chave pública

Parte pública do par criptográfico, incluída no certificado e usada para verificar assinaturas.

## CRL/LCR

Lista de Certificados Revogados publicada por uma autoridade.

## CSP

Cryptographic Service Provider, modelo clássico de provider criptográfico do Windows.

## HSM

Hardware Security Module, equipamento especializado na proteção e operação de chaves criptográficas.

## ICP-Brasil

Infraestrutura de Chaves Públicas Brasileira, cadeia hierárquica de confiança com o ITI como Autoridade Certificadora Raiz.

## KSP/CNG

Key Storage Provider e Cryptography Next Generation, arquitetura criptográfica mais nova do Windows.

## Middleware

Software que permite ao sistema e às aplicações conversar com token, cartão ou serviço criptográfico.

## OCSP

Protocolo de consulta do estado de revogação de um certificado.

## PFX/P12

Extensões comuns de um contêiner PKCS#12, que pode reunir certificado, cadeia e chave privada protegida.

## PIN

Dado de ativação usado para autorizar operações em token ou cartão. Não é a chave privada.

## Provider

Componente que expõe operações criptográficas e acesso a uma chave para o sistema operacional ou aplicação.

## Store

Repositório lógico de certificados do Windows, como o pessoal do usuário atual ou da máquina.

## Token

Dispositivo criptográfico, frequentemente conectado por USB, que protege a chave privada e executa operações sem expô-la como arquivo comum.

## X.509

Padrão usado para representar certificados de chave pública e informações relacionadas.

#include "certradar/signature_test.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>

#include <vector>

namespace certradar {

SignatureTestResult run_synthetic_a1_signature_test() {
    SignatureTestResult result;
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    BCRYPT_KEY_HANDLE key = nullptr;
    NTSTATUS status = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_RSA_ALGORITHM, nullptr, 0);
    if (status < 0) {
        result.native_status = status;
        result.message = "Nao foi possivel abrir o algoritmo RSA.";
        return result;
    }
    status = BCryptGenerateKeyPair(algorithm, &key, 2048, 0);
    if (status >= 0) status = BCryptFinalizeKeyPair(key, 0);

    std::vector<unsigned char> challenge(32);
    if (status >= 0) {
        status = BCryptGenRandom(
            nullptr, challenge.data(), static_cast<ULONG>(challenge.size()),
            BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    }

    BCRYPT_PKCS1_PADDING_INFO padding{BCRYPT_SHA256_ALGORITHM};
    ULONG signature_size = 0;
    if (status >= 0) {
        status = BCryptSignHash(
            key, &padding, challenge.data(), static_cast<ULONG>(challenge.size()),
            nullptr, 0, &signature_size, BCRYPT_PAD_PKCS1);
    }
    std::vector<unsigned char> signature(signature_size);
    if (status >= 0) {
        status = BCryptSignHash(
            key, &padding, challenge.data(), static_cast<ULONG>(challenge.size()),
            signature.data(), signature_size, &signature_size, BCRYPT_PAD_PKCS1);
    }
    if (status >= 0) {
        status = BCryptVerifySignature(
            key, &padding, challenge.data(), static_cast<ULONG>(challenge.size()),
            signature.data(), signature_size, BCRYPT_PAD_PKCS1);
    }

    result.native_status = status;
    result.signed_and_verified = status >= 0;
    result.message = result.signed_and_verified
        ? "Desafio sintetico assinado e verificado localmente."
        : "A assinatura sintetica nao foi verificada.";
    if (!challenge.empty()) SecureZeroMemory(challenge.data(), challenge.size());
    if (!signature.empty()) SecureZeroMemory(signature.data(), signature.size());
    if (key != nullptr) BCryptDestroyKey(key);
    if (algorithm != nullptr) BCryptCloseAlgorithmProvider(algorithm, 0);
    return result;
}

ExternalSignaturePermission external_signature_permission(
    const bool explicit_user_consent) noexcept {
    return explicit_user_consent
        ? ExternalSignaturePermission::authorized
        : ExternalSignaturePermission::blocked;
}

}  // namespace certradar

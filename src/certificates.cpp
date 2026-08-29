#include "certradar/certificates.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define CERT_CHAIN_PARA_HAS_EXTRA_FIELDS
#include <windows.h>
#include <wincrypt.h>

#include <string>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <vector>

namespace certradar {
namespace {

std::string utf8(const std::wstring& value) {
    if (value.empty()) return {};
    const int required = WideCharToMultiByte(
        CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
    if (required <= 0) return {};
    std::string output(static_cast<std::size_t>(required), '\0');
    WideCharToMultiByte(
        CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()),
        output.data(), required, nullptr, nullptr);
    return output;
}

std::string certificate_name(PCCERT_CONTEXT context, const DWORD flags) {
    const DWORD required = CertGetNameStringW(
        context, CERT_NAME_SIMPLE_DISPLAY_TYPE, flags, nullptr, nullptr, 0);
    if (required <= 1) return {};
    std::wstring value(static_cast<std::size_t>(required), L'\0');
    CertGetNameStringW(
        context, CERT_NAME_SIMPLE_DISPLAY_TYPE, flags, nullptr, value.data(), required);
    value.pop_back();
    return utf8(value);
}

std::uint64_t filetime_value(const FILETIME& value) noexcept {
    ULARGE_INTEGER integer{};
    integer.LowPart = value.dwLowDateTime;
    integer.HighPart = value.dwHighDateTime;
    return integer.QuadPart;
}

std::string format_filetime(const FILETIME& value) {
    SYSTEMTIME system{};
    if (FileTimeToSystemTime(&value, &system) == FALSE) return {};
    char output[32]{};
    const int written = std::snprintf(
        output, sizeof(output), "%04u-%02u-%02uT%02u:%02u:%02uZ",
        system.wYear, system.wMonth, system.wDay,
        system.wHour, system.wMinute, system.wSecond);
    return written > 0 ? std::string(output) : std::string{};
}

std::string hexadecimal(const BYTE* bytes, const DWORD count, const bool reverse) {
    std::ostringstream output;
    output << std::uppercase << std::hex << std::setfill('0');
    for (DWORD index = 0; index < count; ++index) {
        const DWORD position = reverse ? count - index - 1U : index;
        output << std::setw(2) << static_cast<unsigned int>(bytes[position]);
    }
    return output.str();
}

std::string certificate_hash(PCCERT_CONTEXT context) {
    DWORD size = 0;
    if (CertGetCertificateContextProperty(context, CERT_HASH_PROP_ID, nullptr, &size) == FALSE) return {};
    std::vector<BYTE> hash(size);
    if (CertGetCertificateContextProperty(context, CERT_HASH_PROP_ID, hash.data(), &size) == FALSE) return {};
    return hexadecimal(hash.data(), size, false);
}

std::vector<std::string> enhanced_usages(PCCERT_CONTEXT context) {
    DWORD size = 0;
    if (CertGetEnhancedKeyUsage(context, 0, nullptr, &size) == FALSE || size == 0) return {};
    std::vector<BYTE> buffer(size);
    auto* const usage = reinterpret_cast<PCERT_ENHKEY_USAGE>(buffer.data());
    if (CertGetEnhancedKeyUsage(context, 0, usage, &size) == FALSE) return {};
    std::vector<std::string> result;
    for (DWORD index = 0; index < usage->cUsageIdentifier; ++index) {
        if (usage->rgpszUsageIdentifier[index] != nullptr) {
            result.emplace_back(usage->rgpszUsageIdentifier[index]);
        }
    }
    return result;
}

void read_private_key_association(PCCERT_CONTEXT context, CertificateRecord& record) {
    DWORD size = 0;
    if (CertGetCertificateContextProperty(
            context, CERT_KEY_PROV_INFO_PROP_ID, nullptr, &size) == FALSE || size == 0) {
        return;
    }
    std::vector<BYTE> buffer(size);
    if (CertGetCertificateContextProperty(
            context, CERT_KEY_PROV_INFO_PROP_ID, buffer.data(), &size) == FALSE) {
        return;
    }
    const auto* const information = reinterpret_cast<const CRYPT_KEY_PROV_INFO*>(buffer.data());
    record.has_private_key_association = true;
    if (information->pwszProvName != nullptr) record.provider = utf8(information->pwszProvName);
    record.provider_kind = classify_provider_kind(information->dwProvType, record.provider);
}

}  // namespace

CertificateValidity classify_certificate_validity(
    const std::uint64_t valid_from,
    const std::uint64_t valid_until,
    const std::uint64_t now) noexcept {
    constexpr std::uint64_t thirty_days = 30ULL * 24ULL * 60ULL * 60ULL * 10'000'000ULL;
    if (now < valid_from) return CertificateValidity::not_yet_valid;
    if (now > valid_until) return CertificateValidity::expired;
    if (valid_until - now <= thirty_days) return CertificateValidity::expiring_soon;
    return CertificateValidity::valid;
}

ProviderKind classify_provider_kind(
    const unsigned long provider_type,
    const std::string& provider_name) noexcept {
    if (provider_name.empty()) return ProviderKind::unknown;
    return provider_type == 0 ? ProviderKind::ksp : ProviderKind::csp;
}

std::vector<ChainIssue> classify_chain_issues(const unsigned long trust_status) {
    std::vector<ChainIssue> issues;
    unsigned long recognized = 0;
    const auto add = [&](const unsigned long flag, const ChainIssue issue) {
        if ((trust_status & flag) != 0) {
            issues.push_back(issue);
            recognized |= flag;
        }
    };
    add(CERT_TRUST_IS_NOT_TIME_VALID, ChainIssue::expired);
    add(CERT_TRUST_IS_UNTRUSTED_ROOT, ChainIssue::untrusted_root);
    add(CERT_TRUST_IS_PARTIAL_CHAIN, ChainIssue::partial_chain);
    add(CERT_TRUST_IS_REVOKED, ChainIssue::revoked);
    add(CERT_TRUST_REVOCATION_STATUS_UNKNOWN, ChainIssue::revocation_unknown);
    add(CERT_TRUST_IS_OFFLINE_REVOCATION, ChainIssue::revocation_unknown);
    add(CERT_TRUST_INVALID_BASIC_CONSTRAINTS, ChainIssue::invalid_basic_constraints);
    if ((trust_status & ~recognized) != 0) issues.push_back(ChainIssue::other);
    return issues;
}

namespace {

ChainEvaluation evaluate_chain(
    const std::vector<std::uint8_t>& encoded_certificate,
    const bool online,
    const std::uint32_t timeout_ms) {
    ChainEvaluation result;
    result.online_requested = online;
    result.requested_timeout_ms = online ? timeout_ms : 0;
    if (encoded_certificate.empty()) {
        result.error_code = ERROR_INVALID_DATA;
        return result;
    }
    PCCERT_CONTEXT context = CertCreateCertificateContext(
        X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
        encoded_certificate.data(), static_cast<DWORD>(encoded_certificate.size()));
    if (context == nullptr) {
        result.error_code = GetLastError();
        if (result.error_code == 0) result.error_code = ERROR_INVALID_DATA;
        return result;
    }

    CERT_CHAIN_PARA parameters{};
    parameters.cbSize = sizeof(parameters);
    parameters.dwUrlRetrievalTimeout = online ? timeout_ms : 0;
    PCCERT_CHAIN_CONTEXT chain = nullptr;
    const DWORD flags = online
        ? CERT_CHAIN_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT
        : CERT_CHAIN_CACHE_ONLY_URL_RETRIEVAL;
    if (CertGetCertificateChain(
            nullptr, context, nullptr, nullptr, &parameters,
            flags, nullptr, &chain) == FALSE) {
        result.error_code = GetLastError();
        CertFreeCertificateContext(context);
        return result;
    }

    result.built = true;
    result.trust_status = chain->TrustStatus.dwErrorStatus;
    result.issues = classify_chain_issues(result.trust_status);
    CertFreeCertificateChain(chain);
    CertFreeCertificateContext(context);
    return result;
}

}  // namespace

ChainEvaluation evaluate_certificate_chain_local(
    const std::vector<std::uint8_t>& encoded_certificate) {
    return evaluate_chain(encoded_certificate, false, 0);
}

ChainEvaluation evaluate_certificate_chain_online(
    const std::vector<std::uint8_t>& encoded_certificate,
    const std::uint32_t timeout_ms) {
    return evaluate_chain(encoded_certificate, true, timeout_ms == 0 ? 1 : timeout_ms);
}

CertificateStoreResult enumerate_personal_certificates(const StoreScope scope) {
    CertificateStoreResult result;
    const DWORD location = scope == StoreScope::current_user
        ? CERT_SYSTEM_STORE_CURRENT_USER : CERT_SYSTEM_STORE_LOCAL_MACHINE;
    HCERTSTORE store = CertOpenStore(
        CERT_STORE_PROV_SYSTEM_W, 0, 0,
        location | CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_READONLY_FLAG,
        L"MY");
    if (store == nullptr) {
        result.error_code = GetLastError();
        return result;
    }

    result.opened = true;
    PCCERT_CONTEXT context = nullptr;
    while ((context = CertEnumCertificatesInStore(store, context)) != nullptr) {
        CertificateRecord record;
        record.scope = scope;
        record.subject = certificate_name(context, 0);
        record.issuer = certificate_name(context, CERT_NAME_ISSUER_FLAG);
        record.serial_number = hexadecimal(
            context->pCertInfo->SerialNumber.pbData,
            context->pCertInfo->SerialNumber.cbData,
            true);
        record.thumbprint = certificate_hash(context);
        record.valid_from = format_filetime(context->pCertInfo->NotBefore);
        record.valid_until = format_filetime(context->pCertInfo->NotAfter);
        FILETIME now{};
        GetSystemTimeAsFileTime(&now);
        record.validity = classify_certificate_validity(
            filetime_value(context->pCertInfo->NotBefore),
            filetime_value(context->pCertInfo->NotAfter),
            filetime_value(now));
        record.enhanced_key_usages = enhanced_usages(context);
        read_private_key_association(context, record);
        record.encoded_certificate.assign(
            context->pbCertEncoded, context->pbCertEncoded + context->cbCertEncoded);
        result.certificates.push_back(std::move(record));
    }
    CertCloseStore(store, 0);
    return result;
}

}  // namespace certradar

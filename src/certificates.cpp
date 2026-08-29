#include "certradar/certificates.hpp"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>

#include <string>

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

}  // namespace

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
        record.encoded_certificate.assign(
            context->pbCertEncoded, context->pbCertEncoded + context->cbCertEncoded);
        result.certificates.push_back(std::move(record));
    }
    CertCloseStore(store, 0);
    return result;
}

}  // namespace certradar

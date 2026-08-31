#pragma once

#include "certradar/certificates.hpp"
#include "certradar/platform.hpp"
#include "certradar/search.hpp"
#include "certradar/smartcard.hpp"

#include <string>
#include <vector>

namespace certradar {

enum class CandidateRevealStatus { ready, no_selection, unsafe_path };

struct CandidateRevealPlan {
    CandidateRevealStatus status{CandidateRevealStatus::no_selection};
    std::filesystem::path path;
};

std::wstring candidate_state_label(CandidateState state);
std::wstring format_candidate_label(const SearchCandidate& candidate);
std::wstring build_search_support_summary(const SearchResult& result);
std::wstring build_search_support_summary(
    const SearchResult& result,
    const WindowsPlatform& platform);
CandidateRevealPlan build_candidate_reveal_plan(
    const SearchResult& result,
    std::size_t selection_index);
std::wstring format_platform_summary(const WindowsPlatform& platform);
std::wstring format_certificate_summary(
    const CertificateRecord& certificate,
    std::size_t display_index);
std::wstring build_certificate_store_support_summary(
    const CertificateStoreResult& result);
std::wstring build_certificate_store_support_summary(
    const CertificateStoreResult& result,
    const WindowsPlatform& platform);
std::vector<std::size_t> build_certificate_display_order(
    const CertificateStoreResult& result);
std::wstring build_a3_support_summary(const A3LocalSnapshot& snapshot);
std::wstring build_a3_support_summary(
    const A3LocalSnapshot& snapshot,
    const WindowsPlatform& platform);

}  // namespace certradar

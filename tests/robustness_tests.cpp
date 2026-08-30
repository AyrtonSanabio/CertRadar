#include "certradar/facts.hpp"
#include "certradar/report.hpp"
#include "certradar/search.hpp"

#include <doctest/doctest.h>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace {

class DeterministicBytes {
public:
    std::uint32_t next() noexcept {
        state_ ^= state_ << 13U;
        state_ ^= state_ >> 17U;
        state_ ^= state_ << 5U;
        return state_;
    }

    std::string text(const std::size_t maximum_size) {
        const auto size = static_cast<std::size_t>(next()) % (maximum_size + 1);
        std::string value(size, '\0');
        for (auto& character : value) character = static_cast<char>(next() & 0xffU);
        return value;
    }

    std::vector<unsigned char> blob(const std::size_t maximum_size) {
        const auto size = static_cast<std::size_t>(next()) % (maximum_size + 1);
        std::vector<unsigned char> value(size);
        for (auto& byte : value) byte = static_cast<unsigned char>(next() & 0xffU);
        return value;
    }

private:
    std::uint32_t state_{0x43525452U};
};

class TemporaryFuzzFile {
public:
    TemporaryFuzzFile() {
        const auto unique = std::to_string(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
        path_ = std::filesystem::temp_directory_path() / ("certradar-fuzz-" + unique + ".pfx");
    }
    ~TemporaryFuzzFile() {
        std::error_code ignored;
        std::filesystem::remove(path_, ignored);
    }
    const std::filesystem::path& path() const noexcept { return path_; }
    void overwrite(const std::vector<unsigned char>& bytes) const {
        std::ofstream output(path_, std::ios::binary | std::ios::trunc);
        output.write(
            reinterpret_cast<const char*>(bytes.data()),
            static_cast<std::streamsize>(bytes.size()));
    }

private:
    std::filesystem::path path_;
};

}  // namespace

TEST_CASE("deterministic JSON mutations are rejected or contained without unknown exceptions") {
    DeterministicBytes generator;
    for (int iteration = 0; iteration < 5000; ++iteration) {
        const auto input = generator.text(512);
        try {
            static_cast<void>(certradar::deserialize_facts(input));
        } catch (const std::exception&) {
            continue;
        } catch (...) {
            FAIL_CHECK("deserialize_facts emitted a non-standard exception");
        }
    }
}

TEST_CASE("deterministic privacy mutations never produce an empty identifier result") {
    DeterministicBytes generator;
    for (int iteration = 0; iteration < 5000; ++iteration) {
        const auto input = generator.text(512);
        const auto identifier = certradar::mask_identifier(input);
        const auto path = certradar::mask_path(input);
        CHECK(identifier.rfind("***", 0) == 0);
        CHECK_FALSE(path.empty());
    }
}

TEST_CASE("bounded random PKCS12 blobs stay inside the safe candidate state machine") {
    DeterministicBytes generator;
    TemporaryFuzzFile candidate;
    for (int iteration = 0; iteration < 500; ++iteration) {
        candidate.overwrite(generator.blob(2048));
        const auto state = certradar::inspect_pkcs12_container(candidate.path(), 4096);
        CHECK((state == certradar::CandidateState::recognized ||
               state == certradar::CandidateState::invalid));
    }
}

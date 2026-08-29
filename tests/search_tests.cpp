#include "certradar/search.hpp"

#include <doctest/doctest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <future>
#include <string>
#include <thread>

namespace {

class TemporaryDirectory {
public:
    TemporaryDirectory() {
        const auto unique = std::to_string(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
        path_ = std::filesystem::temp_directory_path() / ("certradar-tests-" + unique);
        std::filesystem::create_directories(path_);
    }

    ~TemporaryDirectory() { std::filesystem::remove_all(path_); }
    const std::filesystem::path& path() const noexcept { return path_; }

    void create_file(const std::filesystem::path& relative, const std::string& content = "test") {
        const auto destination = path_ / relative;
        std::filesystem::create_directories(destination.parent_path());
        std::ofstream output(destination, std::ios::binary);
        output << content;
    }

private:
    std::filesystem::path path_;
};

}  // namespace

TEST_CASE("A1 extensions are recognized without case sensitivity") {
    CHECK(certradar::has_a1_extension("certificado.pfx"));
    CHECK(certradar::has_a1_extension("CERTIFICADO.PFX"));
    CHECK(certradar::has_a1_extension("backup.P12"));
    CHECK_FALSE(certradar::has_a1_extension("publico.cer"));
    CHECK_FALSE(certradar::has_a1_extension("enganoso.pfx.exe"));
}

TEST_CASE("priority search finds only direct PFX and P12 candidates") {
    TemporaryDirectory fixture;
    fixture.create_file("primeiro.PFX");
    fixture.create_file("segundo.p12");
    fixture.create_file("ignorado.txt");
    fixture.create_file("subpasta/depois.pfx");

    const auto result = certradar::search_files({fixture.path()});

    CHECK(result.status == certradar::ScanStatus::completed);
    CHECK(result.directories_visited == 1);
    REQUIRE(result.candidates.size() == 2);
    CHECK(result.candidates[0].path.filename() == "primeiro.PFX");
    CHECK(result.candidates[1].path.filename() == "segundo.p12");
}

TEST_CASE("recursive search visits the current profile iteratively") {
    TemporaryDirectory fixture;
    fixture.create_file("nivel-1/nivel-2/certificado.pfx");
    fixture.create_file("nivel-1/outro.p12");

    certradar::SearchOptions options;
    options.recursive = true;
    const auto result = certradar::search_files({fixture.path()}, options);

    CHECK(result.status == certradar::ScanStatus::completed);
    CHECK(result.directories_visited == 3);
    REQUIRE(result.candidates.size() == 2);
    CHECK(result.candidates[0].path.filename() == "outro.p12");
    CHECK(result.candidates[1].path.filename() == "certificado.pfx");
}

TEST_CASE("directory safety policy refuses Windows reparse points") {
    constexpr std::uint32_t directory = 0x00000010;
    constexpr std::uint32_t reparse_point = 0x00000400;

    CHECK(certradar::should_traverse_directory(directory));
    CHECK_FALSE(certradar::should_traverse_directory(directory | reparse_point));
    CHECK_FALSE(certradar::should_traverse_directory(reparse_point));
}

TEST_CASE("unreadable or missing roots do not abort remaining roots") {
    TemporaryDirectory fixture;
    fixture.create_file("encontrado.pfx");
    const auto missing = fixture.path() / "nao-existe";

    const auto result = certradar::search_files({missing, fixture.path()});

    CHECK(result.status == certradar::ScanStatus::completed);
    CHECK(result.errors == 1);
    REQUIRE(result.candidates.size() == 1);
    CHECK(result.candidates.front().path.filename() == "encontrado.pfx");
}

TEST_CASE("cancellation stops new directory work and keeps partial results") {
    TemporaryDirectory fixture;
    fixture.create_file("primeiro.pfx");
    fixture.create_file("subpasta/segundo.pfx");
    certradar::SearchOptions options;
    options.recursive = true;
    certradar::SearchControl control;

    const auto result = certradar::search_files(
        {fixture.path()}, options, control,
        [&control](const certradar::SearchProgress& progress) {
            if (progress.directories_visited == 1) control.cancel();
        });

    CHECK(result.status == certradar::ScanStatus::cancelled);
    CHECK(result.directories_visited == 1);
    CHECK(result.candidates.size() <= 1);
}

TEST_CASE("paused search resumes cooperatively") {
    TemporaryDirectory fixture;
    fixture.create_file("certificado.pfx");
    certradar::SearchControl control;
    control.request_pause();

    auto pending = std::async(std::launch::async, [&] {
        return certradar::search_files(
            {fixture.path()}, certradar::SearchOptions{}, control, {});
    });
    CHECK(pending.wait_for(std::chrono::milliseconds(50)) == std::future_status::timeout);
    control.resume();
    CHECK(pending.wait_for(std::chrono::seconds(2)) == std::future_status::ready);
    CHECK(pending.get().status == certradar::ScanStatus::completed);
}

TEST_CASE("container inspection rejects oversized and malformed candidates without a password") {
    TemporaryDirectory fixture;
    fixture.create_file("malformado.pfx", "isto nao e um PKCS12");

    CHECK(certradar::inspect_pkcs12_container(fixture.path() / "malformado.pfx", 4) ==
          certradar::CandidateState::too_large);
    CHECK(certradar::inspect_pkcs12_container(fixture.path() / "malformado.pfx", 1024) ==
          certradar::CandidateState::invalid);
}

TEST_CASE("validated search classifies a malformed candidate instead of executing it") {
    TemporaryDirectory fixture;
    fixture.create_file("suspeito.p12", "dados aleatorios");
    certradar::SearchOptions options;
    options.validate_containers = true;

    const auto result = certradar::search_files({fixture.path()}, options);

    REQUIRE(result.candidates.size() == 1);
    CHECK(result.candidates.front().state == certradar::CandidateState::invalid);
}

TEST_CASE("search deduplicates hard links and repeated roots while preserving discovery order") {
    TemporaryDirectory fixture;
    fixture.create_file("original.pfx");
    std::filesystem::create_hard_link(
        fixture.path() / "original.pfx", fixture.path() / "copia.pfx");

    const auto result = certradar::search_files({fixture.path(), fixture.path()});

    REQUIRE(result.candidates.size() == 1);
    CHECK(result.candidates.front().path.filename() == "copia.pfx");
    CHECK(result.directories_visited == 1);
}

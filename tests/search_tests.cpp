#include "certradar/search.hpp"

#include <doctest/doctest.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

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

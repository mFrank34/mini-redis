#include <catch2/catch_test_macros.hpp>
#include "../src/Wal.h"
#include <cstdio>

// Uses a dedicated test file path so these tests never touch your real log.txt,
// and cleans up after itself so repeated test runs start fresh.

namespace {
    struct TempWalFile {
        std::string path = "test_wal_temp.txt";
        ~TempWalFile() {
            std::remove(path.c_str());
            std::remove((path + ".tmp").c_str());
        }
    };
}

TEST_CASE("write then read round-trips a single entry", "[wal]") {
    TempWalFile tmp;
    Wal wal(tmp.path);

    std::unordered_map<std::string, std::string> data = { {"foo", "bar"} };
    wal.write(data);

    auto result = wal.read();
    REQUIRE(result.size() == 1);
    REQUIRE(result.at("foo") == "bar");
}

TEST_CASE("write then read round-trips multiple entries", "[wal]") {
    TempWalFile tmp;
    Wal wal(tmp.path);

    std::unordered_map<std::string, std::string> data = {
        {"a", "1"}, {"b", "2"}, {"c", "3"}
    };
    wal.write(data);

    auto result = wal.read();
    REQUIRE(result.size() == 3);
    REQUIRE(result.at("a") == "1");
    REQUIRE(result.at("b") == "2");
    REQUIRE(result.at("c") == "3");
}

TEST_CASE("a second write fully replaces the first (acts as a snapshot, not a log)", "[wal]") {
    TempWalFile tmp;
    Wal wal(tmp.path);

    wal.write({ {"foo", "bar"} });
    wal.write({ {"baz", "qux"} });   // simulates a DEL of "foo" followed by re-snapshotting

    auto result = wal.read();
    REQUIRE(result.size() == 1);
    REQUIRE_FALSE(result.contains("foo"));
    REQUIRE(result.at("baz") == "qux");
}

TEST_CASE("reading a file that doesn't exist yet returns an empty map", "[wal]") {
    TempWalFile tmp;   // never written to
    Wal wal(tmp.path);

    auto result = wal.read();
    REQUIRE(result.empty());
}

TEST_CASE("writing an empty map produces an empty read-back", "[wal]") {
    TempWalFile tmp;
    Wal wal(tmp.path);

    wal.write({});
    auto result = wal.read();

    REQUIRE(result.empty());
}

#include <catch2/catch_test_macros.hpp>
#include "../src/Store.h"

TEST_CASE("set then get returns the value", "[store]") {
    Store store(1000);
    store.set("foo", "bar");
    auto val = store.get("foo");

    REQUIRE(val.has_value());
    REQUIRE(*val == "bar");
}

TEST_CASE("get on a missing key returns nullopt", "[store]") {
    Store store(1000);
    auto val = store.get("does_not_exist");
    REQUIRE_FALSE(val.has_value());
}

TEST_CASE("set overwrites an existing key", "[store]") {
    Store store(1000);
    store.set("foo", "bar");
    store.set("foo", "baz");

    auto val = store.get("foo");
    REQUIRE(val.has_value());
    REQUIRE(*val == "baz");   // catches the try_emplace-no-overwrite bug if it ever comes back
}

TEST_CASE("del removes a key and returns true", "[store]") {
    Store store(1000);
    store.set("foo", "bar");

    REQUIRE(store.del("foo"));
    REQUIRE_FALSE(store.get("foo").has_value());
}

TEST_CASE("del on a missing key returns false", "[store]") {
    Store store(1000);
    REQUIRE_FALSE(store.del("nope"));
}

TEST_CASE("save returns everything that was set", "[store]") {
    Store store(1000);
    store.set("a", "1");
    store.set("b", "2");
    store.set("c", "3");

    auto snapshot = store.save();

    REQUIRE(snapshot.size() == 3);
    REQUIRE(snapshot.at("a") == "1");
    REQUIRE(snapshot.at("b") == "2");
    REQUIRE(snapshot.at("c") == "3");
}

TEST_CASE("load repopulates the store from a map", "[store]") {
    Store store(1000);
    std::unordered_map<std::string, std::string> data = {
        {"x", "10"}, {"y", "20"}
    };

    store.load(data);

    REQUIRE(store.get("x") == "10");
    REQUIRE(store.get("y") == "20");
}

TEST_CASE("save after load matches what was loaded", "[store]") {
    Store store(1000);
    std::unordered_map<std::string, std::string> data = {
        {"x", "10"}, {"y", "20"}, {"z", "30"}
    };

    store.load(data);
    auto snapshot = store.save();

    REQUIRE(snapshot.size() == data.size());
    for (const auto& [k, v] : data) {
        REQUIRE(snapshot.at(k) == v);
    }
}

// Once LRU eviction is wired in with a configurable cap, tests like this
// confirm eviction actually kicks in and respects recency:
//
// TEST_CASE("LRU evicts the least recently used entry when full", "[store][lru]") {
//     Store store(2);              // cap of 2 entries
//     store.set("a", "1");
//     store.set("b", "2");
//     store.get("a");              // touch "a" -> now most recently used
//     store.set("c", "3");         // should evict "b", the least recently used
//
//     REQUIRE(store.get("a").has_value());
//     REQUIRE_FALSE(store.get("b").has_value());
//     REQUIRE(store.get("c").has_value());
// }

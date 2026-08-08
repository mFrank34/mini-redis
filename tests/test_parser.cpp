#include <catch2/catch_test_macros.hpp>
#include "../src/Parser.h"

TEST_CASE("parses a SET command with two arguments", "[parser]") {
    Command cmd = parse("SET foo bar");

    REQUIRE(cmd.command == "SET");
    REQUIRE(cmd.arguments.size() == 2);
    REQUIRE(cmd.arguments[0] == "foo");
    REQUIRE(cmd.arguments[1] == "bar");
}

TEST_CASE("parses a GET command with one argument", "[parser]") {
    Command cmd = parse("GET foo");

    REQUIRE(cmd.command == "GET");
    REQUIRE(cmd.arguments.size() == 1);
    REQUIRE(cmd.arguments[0] == "foo");
}

TEST_CASE("parses a DEL command with one argument", "[parser]") {
    Command cmd = parse("DEL foo");

    REQUIRE(cmd.command == "DEL");
    REQUIRE(cmd.arguments.size() == 1);
    REQUIRE(cmd.arguments[0] == "foo");
}

TEST_CASE("command name is normalized to uppercase", "[parser]") {
    Command cmd = parse("set foo bar");
    REQUIRE(cmd.command == "SET");

    Command cmd2 = parse("Get foo");
    REQUIRE(cmd2.command == "GET");
}

TEST_CASE("command with no arguments parses with an empty arguments list", "[parser]") {
    Command cmd = parse("PING");

    REQUIRE(cmd.command == "PING");
    REQUIRE(cmd.arguments.empty());
}

TEST_CASE("extra arguments beyond what a command needs are still captured", "[parser]") {
    // parser's job is just tokenizing — arg-count validation belongs to the dispatcher, not here
    Command cmd = parse("GET hello world and more");

    REQUIRE(cmd.command == "GET");
    REQUIRE(cmd.arguments.size() == 4);
}

TEST_CASE("empty input produces an empty command", "[parser]") {
    Command cmd = parse("");

    REQUIRE(cmd.command.empty());
    REQUIRE(cmd.arguments.empty());
}

TEST_CASE("extra whitespace between tokens is handled", "[parser]") {
    Command cmd = parse("SET   foo    bar");

    REQUIRE(cmd.command == "SET");
    REQUIRE(cmd.arguments.size() == 2);
    REQUIRE(cmd.arguments[0] == "foo");
    REQUIRE(cmd.arguments[1] == "bar");
}

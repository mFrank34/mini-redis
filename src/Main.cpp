/*
 * Key store project
 */

#include <iostream>
#include <algorithm>
#include "Parser.h"
#include "Store.h"

int main() {
    std::cout << "-- Application Running --" << std::endl;
    Store store;

    bool runtime = true;
    while (runtime) {
        std::cout << "> ";
        std::string line;
        std::getline(std::cin, line);

        if (line == "exit") {
            runtime = false;
            continue;   // or break/return, depending on your loop structure
        }

        Command cmd = parse(line);

        // after parse_command(line):
        std::transform(
            cmd.command.begin(),
            cmd.command.end(),
            cmd.command.begin(),
            ::toupper);

        if (cmd.command == "SET") {
            if (cmd.arguments.size() != 2) {
                std::cout << "ERR wrong number of arguments for SET\n";
            } else {
                store.set(cmd.arguments[0], cmd.arguments[1]);
                std::cout << "OK\n";
            }
        } else if (cmd.command == "GET") {
            if (cmd.arguments.size() != 1) {
                std::cout << "ERR wrong number of arguments for GET\n";
            } else {
                auto val = store.get(cmd.arguments[0]);
                std::cout << (val ? *val : "(nil)") << "\n";
            }
        } else if (cmd.command == "DEL") {
            if (cmd.arguments.size() != 1) {
                std::cout << "ERR wrong number of arguments for DEL\n";
            } else {
                std::cout << (store.del(cmd.arguments[0]) ? "OK\n" : "(nil)\n");
            }
        } else {
            std::cout << "ERR unknown command\n";
        }
    }

    return 0;
}
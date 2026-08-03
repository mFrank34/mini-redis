//
// Created by frank on 02/08/2026.
//

#include "Parser.h"

#include <sstream>

Command parse(const std::string &line) {
    Command cmd;
    std::istringstream iss(line);
    iss >> cmd.command;

    std::string arg;
    while (iss >> arg) {
        cmd.arguments.push_back(arg);
    }
    return cmd;
}

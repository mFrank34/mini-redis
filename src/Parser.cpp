//
// Created by frank on 02/08/2026.
//

#include "Parser.h"

#include <algorithm>
#include <sstream>

Command parse(const std::string &line) {
    Command cmd;
    std::istringstream iss(line);
    iss >> cmd.command;
    std::transform(cmd.command.begin(), cmd.command.end(), cmd.command.begin(), ::toupper);
    std::string arg;
    while (iss >> arg) {
        cmd.arguments.push_back(arg);
    }
    return cmd;
}

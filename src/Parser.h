//
// Created by frank on 02/08/2026.
//

#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>

struct Command {
    std::string command;
    std::pmr::vector<std::string> arguments;
};

Command parse(const std::string& line);

#endif //PARSER_H

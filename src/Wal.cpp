//
// Created by frank on 02/08/2026.
//

#include "Wal.h"

#include <fstream>wha
#include <valarray>
#include <vector>


std::unordered_map<std::string, std::string> Wal::read() const
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::unordered_map<std::string, std::string> data;
    std::ifstream file(path);

    std::string line;
    while (getline(file, line))
    {
        // key;data
        std::pair<std::string, std::string> pair = splitLine(line);
        data.try_emplace(pair.first, pair.second);
    }
    file.close();
    return data;
}

void Wal::write(const std::unordered_map<std::string, std::string>& data) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::string temp_path = path + ".tmp";
    {
        std::ofstream log(temp_path);
        for (auto& line : data)
        {
            log << line.first << DELIM << line.second << "\n";
        }
    }
    std::rename(temp_path.c_str(), path.c_str());
}

std::pair<std::string, std::string> Wal::splitLine(const std::string& line)
{
    size_t pos = line.find(DELIM);

    if (pos == std::string::npos)
    {
        return std::make_pair(line, "");
    }
    std::string first = line.substr(0, pos);
    std::string second = line.substr(pos + 1);
    return {first, second};
}

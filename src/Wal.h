//
// Created by frank on 02/08/2026.
//

#ifndef WAL_H
#define WAL_H

#include <mutex>
#include <string>
#include <unordered_map>


class Wal
{
public:
    Wal() = default;
    ~Wal() = default;

    /**
     * system to reading in the data into the registry on restart
     * @return Returns the data that was stored from last lower cycle
     */
    [[nodiscard]] std::unordered_map<std::string, std::string> read() const;

    /**
     * creates a data coppy of in-memory storage.
     * @param data data writing to application
     */
    void write(const std::unordered_map<std::string, std::string>& data) const;

private:
    /**
     * line to break into 2 parts
     * @param line current line to process
     * @return line broken into 2.
     */
    static std::pair<std::string, std::string> splitLine(const std::string& line);

    std::string path = "log.txt";
    static constexpr char DELIM = '\x1F';
    mutable std::mutex mutex_;
};


#endif //WAL_H

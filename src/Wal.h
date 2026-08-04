//
// Created by frank on 02/08/2026.
//

#ifndef WAL_H
#define WAL_H

#include <string>


class Wal {
public:
    Wal(std::string &path);
    void save();
    void load();

private:
    bool read();
    bool write();

    std::string path = "log.txt";

};


#endif //WAL_H

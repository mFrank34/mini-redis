cmake_minimum_required(VERSION 3.20)
project(key_store)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(key_store
        src/Main.cpp
        src/Server.cpp
        src/Server.h
        src/Store.cpp
        src/Store.h
        src/Parser.cpp
        src/Parser.h
        src/Wal.cpp
        src/Wal.h
)

find_package(Threads REQUIRED)
target_link_libraries(key_store PRIVATE Threads::Threads)
cmake_minimum_required(VERSION 3.20)
project(key_store)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- main executable (unchanged) ---
add_executable(key_store
    src/main.cpp
    src/Server.cpp
    src/Server.h
    src/Store.cpp
    src/Store.h
    src/Parser.cpp
    src/Parser.h
    src/Wal.cpp
    src/Wal.h
    src/Thread.cpp
    src/Thread.h
)

find_package(Threads REQUIRED)
target_link_libraries(key_store PRIVATE Threads::Threads)

# --- Catch2 ---
include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(Catch2)

# --- test executable ---
add_executable(tests
    tests/test_store.cpp
    src/Store.cpp
    src/Store.h
    src/Wal.cpp
    src/Wal.h
    src/Parser.cpp
    src/Parser.h
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain Threads::Threads)

# lets CTest discover and run each Catch2 TEST_CASE individually
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(tests)
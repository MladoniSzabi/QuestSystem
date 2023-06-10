cmake_minimum_required(VERSION 3.20)
project(QuestSystem VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(out
    src/main.cpp

    vendor/src/sqlite/sqlite3.c

    src/src/QuestSystem.cpp
)

target_include_directories(out PUBLIC src/include)
target_include_directories(out PUBLIC vendor/include)
set(HEADER_FILES src/include/QuestSystem.hpp)

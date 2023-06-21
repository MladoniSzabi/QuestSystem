cmake_minimum_required(VERSION 3.20)
project(QuestSystem VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(out STATIC

    vendor/src/sqlite/sqlite3.c

    src/src/QuestSystem.cpp
)

add_subdirectory(editor)
add_subdirectory(test)

include_directories(
    src/include
    vendor/include
)

target_include_directories(out PUBLIC src/include)
target_include_directories(out PUBLIC vendor/include)
set(HEADER_FILES src/include/QuestSystem.hpp)

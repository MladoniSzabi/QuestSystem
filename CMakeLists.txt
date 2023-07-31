cmake_minimum_required(VERSION 3.20)
project(QuestSystem VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(questEditor STATIC

    vendor/src/sqlite/sqlite3.c

    library/src/QuestSystem.cpp
    library/src/Requirement.cpp
)

add_subdirectory(editor)
add_subdirectory(test)

include_directories(
    library/include
    vendor/include
)

target_include_directories(questEditor PUBLIC src/include)
target_include_directories(questEditor PUBLIC vendor/include)
set(HEADER_FILES library/include/QuestSystem.hpp)

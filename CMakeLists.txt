cmake_minimum_required(VERSION 3.20)
project(QuestSystem VERSION 0.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src)
add_subdirectory(src)
add_subdirectory(test)
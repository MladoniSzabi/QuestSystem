cmake_minimum_required(VERSION 3.13)
project(editor)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
    message("opengl found")
    message("include dir: ${OPENGL_INCLUDE_DIR}")
    message("link libraries: ${OPENGL_gl_LIBRARY}")
else (OPENGL_FOUND)
    message("opengl not found")
endif()

include_directories(
    include
    ../src/include
    ../vendor/include/
    ../vendor/include/imgui
    ${OPENGL_gl_LIBRARY}
)

add_executable(editor

    src/main.cpp
    src/ImGuiManager.cpp
    src/QuestLayer.cpp
    src/MainLayer.cpp
    src/StageLayer.cpp
    src/util.cpp

    ../vendor/src/imgui/imgui.cpp
    ../vendor/src/imgui/imgui_demo.cpp
    ../vendor/src/imgui/imgui_draw.cpp
    ../vendor/src/imgui/imgui_tables.cpp
    ../vendor/src/imgui/imgui_widgets.cpp
    ../vendor/src/imgui/imgui_stdlib.cpp

    ../vendor/src/imgui/imgui_impl_opengl3.cpp
    ../vendor/src/imgui/imgui_impl_glfw.cpp
)

target_link_libraries(editor GL)
target_link_libraries(editor glfw)

target_link_libraries(editor ${CMAKE_CURRENT_LIST_DIR}/../libout.a)
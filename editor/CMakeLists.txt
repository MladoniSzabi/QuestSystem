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
    ../library/include
    ../vendor
    ${OPENGL_gl_LIBRARY}
)

add_executable(editor

    src/main.cpp
    src/ImGuiManager.cpp
    src/QuestLayer.cpp
    src/MainLayer.cpp
    src/StageLayer.cpp
    src/util.cpp
    src/RequirementsTableLayer.cpp

    ../vendor/imgui/imgui.cpp
    ../vendor/imgui/imgui_demo.cpp
    ../vendor/imgui/imgui_draw.cpp
    ../vendor/imgui/imgui_tables.cpp
    ../vendor/imgui/imgui_widgets.cpp
    ../vendor/imgui/imgui_stdlib.cpp

    ../vendor/imgui/imgui_impl_opengl3.cpp
    ../vendor/imgui/imgui_impl_glfw.cpp
)

target_link_libraries(editor GL)
target_link_libraries(editor glfw)

target_link_libraries(editor ${CMAKE_CURRENT_LIST_DIR}/../libquestEditor.a)
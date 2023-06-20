#ifndef IMGUIMANAGER_HPP_
#define IMGUIMANAGER_HPP_

#include <stdio.h>
#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "ImGuiLayer.hpp"

class ImGuiManager
{

private:
    GLFWwindow *window = nullptr;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::vector<ImGuiLayer *> layers;

public:
    ~ImGuiManager();

    void init();
    void draw();
    void clean();

    void addLayer(ImGuiLayer *);
};

#endif // IMGUIMANAGER_HPP_
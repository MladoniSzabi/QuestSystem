#include "ImGuiManager.hpp"
#include "MainLayer.hpp"

int main()
{
    ImGuiManager context;
    MainLayer ml;
    context.init();
    context.addLayer(&ml);
    context.draw();
    return 0;
}
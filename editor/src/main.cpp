#include "ImGuiManager.hpp"
#include "ImGuiLayer.hpp"
#include "QuestLayer.hpp"

int main()
{
    ImGuiManager context;
    QuestLayer l;
    context.init();
    context.addLayer(&l);
    context.draw();
    return 0;
}
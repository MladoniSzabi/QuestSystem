#include "MainLayer.hpp"

#include <iostream>

#include "QuestLayer.hpp"
#include "StageLayer.hpp"

MainLayer::MainLayer()
{
    _fileDialog.SetTitle("Open Quest Database");
    _fileDialog.SetTypeFilters({".db"});

    std::shared_ptr<QuestLayer> ql = std::make_shared<QuestLayer>(std::ref(_db), std::ref(_qs));
    addEventListener("dbChanged", ql.get());
    //_eventListeners["dbChanged"].push_back(ql.get());
    _childLayers.push_back({"quests", ql});

    std::shared_ptr<StageLayer> sl = std::make_shared<StageLayer>(std::ref(_db), std::ref(_qs));
    ql->addEventListener("selectedQuest", sl.get());
    _childLayers.push_back({"stages", sl});
}

MainLayer::~MainLayer()
{
    if (_db)
    {
        sqlite3_close(_db);
    }
}

void MainLayer::draw()
{
    bool showDemoWindow = true;
    ImGui::ShowDemoWindow(&showDemoWindow);

    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Quest Editor", nullptr, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
                _fileDialog.Open();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (ImGui::BeginTabBar("tabsBar", ImGuiTabBarFlags_Reorderable |
                                          ImGuiTabBarFlags_FittingPolicyMask_))
    {
        for (auto item : _childLayers)
        {
            if (ImGui::BeginTabItem(item.first.c_str()))
            {
                item.second->draw();
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }

    ImGui::End();

    _fileDialog.Display();

    if (_fileDialog.HasSelected())
    {
        if (_db != nullptr)
        {
            sqlite3_close(_db);
        }
        sqlite3_open(_fileDialog.GetSelected().string().c_str(), &_db);
        std::cout << "Opened database " << _fileDialog.GetSelected().string() << std::endl;
        _fileDialog.ClearSelected();
        _qs.setDatabase(_db);
        dispatchEvent("dbChanged", _db);
    }
}
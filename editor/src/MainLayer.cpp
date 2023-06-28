#include "MainLayer.hpp"

#include <iostream>
#include "QuestLayer.hpp"

MainLayer::MainLayer()
{
    _fileDialog.SetTitle("Open Quest Database");
    _fileDialog.SetTypeFilters({".db"});

    std::shared_ptr<QuestLayer> ql = std::make_shared<QuestLayer>(std::ref(_db), std::ref(_qs));
    _eventListeners["dbChanged"].push_back(ql.get());
    _childLayers["quests"] = ql;
}

MainLayer::~MainLayer()
{
    if (_db)
    {
        sqlite3_close(_db);
    }
}

void MainLayer::dispatchEvent(const std::string &eventName, void *eventData)
{
    if (_eventListeners.find(eventName) == _eventListeners.end())
    {
        std::cout << "Event: " << eventName << " has no event listeners" << std::endl;
        return;
    }
    for (EventListener *l : _eventListeners[eventName])
    {
        l->handleEvent(eventName, eventData);
    }
}

void MainLayer::addEventListener(const std::string &eventName, EventListener *listener)
{
    _eventListeners[eventName].push_back(listener);
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

    if (ImGui::BeginTabBar("tabsBar"))
    {
        for (auto const &item : _childLayers)
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
        //_quests = _qs.getAllQuests();
    }
}
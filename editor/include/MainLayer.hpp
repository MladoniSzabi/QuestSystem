#ifndef MAINLAYER_HPP_
#define MAINLAYER_HPP_

#include <vector>
#include <unordered_map>

#include <sqlite/sqlite3.h>
#include <imgui/imgui.h>
#include <imguifilebrowser/imguifilebrowser.hpp>

#include "ImGuiLayer.hpp"
#include "EventEmitter.hpp"
#include "QuestSystem.hpp"

class MainLayer : public ImGuiLayer, public EventEmitter<sqlite3 *>
{
private:
    sqlite3 *_db = nullptr;
    ImGui::FileBrowser _fileDialog;
    QuestSystem _qs;
    long _selectedQuest = -1;
    std::vector<std::pair<std::string, std::shared_ptr<ImGuiLayer>>> _childLayers;

public:
    MainLayer();
    ~MainLayer();
    void draw() override;
};

#endif // MAINLAYER_HPP_
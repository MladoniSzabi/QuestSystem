#ifndef QUESTLAYER_HPP_
#define QUESTLAYER_HPP_

#include <vector>

#include <sqlite/sqlite3.h>
#include <imgui/imgui.h>
#include <imguifilebrowser/imguifilebrowser.hpp>

#include "ImGuiLayer.hpp"
#include "QuestSystem.hpp"

class QuestLayer : public ImGuiLayer
{
private:
    bool _showCreateQuestWindow = false;
    std::vector<std::string> _quests;
    sqlite3 *_db = nullptr;
    ImGui::FileBrowser _fileDialog;

    void queryDb();

public:
    QuestLayer();
    ~QuestLayer();
    void draw() override;
};

#endif // QUESTLAYER_HPP_
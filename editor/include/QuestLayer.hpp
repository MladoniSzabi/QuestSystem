#ifndef QUESTLAYER_HPP_
#define QUESTLAYER_HPP_

#include <vector>

#include <sqlite/sqlite3.h>
#include <imgui/imgui.h>
#include <imguifilebrowser/imguifilebrowser.hpp>

#include "ImGuiLayer.hpp"
#include "QuestSystem.hpp"
#include "Quest.hpp"

class QuestLayer : public ImGuiLayer
{
private:
    bool _showCreateQuestWindow = false;
    sqlite3 *_db = nullptr;
    ImGui::FileBrowser _fileDialog;
    std::vector<Quest> _quests;
    QuestSystem _qs;

    void renderQuest(Quest &);
    void renderQuestLeafNode(const std::string &label, std::string &value, long id);

public:
    QuestLayer();
    ~QuestLayer();
    void draw() override;
};

#endif // QUESTLAYER_HPP_
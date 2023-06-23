#include "QuestLayer.hpp"

#include <iostream>
#include <imgui/imgui_stdlib.h>

bool isNumber(const std::string &s)
{
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c)
                                      { return !std::isdigit(c); }) == s.end();
}

QuestLayer::QuestLayer()
{
    _fileDialog.SetTitle("Open Quest Database");
    _fileDialog.SetTypeFilters({".db"});
}

QuestLayer::~QuestLayer()
{
    sqlite3_close(_db);
}

void QuestLayer::renderQuestLeafNode(const std::string &label, std::string &value, long id)
{
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
    ImGui::TreeNodeEx("Field", flags, "%s", label.c_str());
    ImGui::TableSetColumnIndex(1);

    // TODO: this is slow and should not happen every frame, find a way to batch sql queries
    if (ImGui::InputText("", &value))
    {
        char *errmsg = nullptr;
        std::string escapedValue = value;
        size_t pos = escapedValue.find("\"", pos);
        while (pos != std::string::npos)
        {
            escapedValue.replace(pos, 1, "\\\"");
            pos = escapedValue.find("\"", pos + 1);
        }
        std::string sql = "UPDATE Quest SET " + label + " = \"" + escapedValue + "\" WHERE Id=" + std::to_string(id);
        int rc = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
        if (rc)
        {
            std::cout << "Error updating " << label << ": " << errmsg << " | " << sql << std::endl;
        }
    }
}

bool QuestLayer::renderQuest(Quest &quest)
{
    bool isDeleted = false;
    ImGui::PushID(quest.id);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::AlignTextToFramePadding();
    bool node_open = ImGui::TreeNode("Quest", "%s", quest.name.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%s", "");

    if (node_open)
    {
        // render name
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("Delete"))
        {
            char *errmsg = nullptr;
            std::string sql = "DELETE FROM Quest WHERE Id=" + std::to_string(quest.id);
            int rc = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
            if (rc)
            {
                std::cout << "There was an error deleting this quest: " << errmsg << std::endl;
            }
            else
            {
                isDeleted = true;
            }
        }
        ImGui::TableNextRow();
        renderQuestLeafNode("name", quest.name, quest.id);
        ImGui::TableNextRow();
        renderQuestLeafNode("description", quest.description, quest.id);
        ImGui::TreePop();
    }

    ImGui::PopID();
    return isDeleted;
}

static int createQuestCallback(void *ptr, int rowCount, char **values, char **rowNames)
{
    long *newRow = (long *)newRow;
    *newRow = atol(values[0]);
    std::cout << "Creating new Quest: " << *newRow << std::endl;
    return 0;
}

void QuestLayer::draw()
{
    bool showDemoWindow = true;
    ImGui::ShowDemoWindow(&showDemoWindow);
    _fileDialog.Display();

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

    if (_db != nullptr)
    {
        if (ImGui::Button("Add Quest"))
        {
            char *errmsg = nullptr;
            long newRow = 0;
            if (sqlite3_exec(
                    _db, "INSERT INTO Quest(Name, Description) Values('', '') returning Id", createQuestCallback, (void *)&newRow, &errmsg))
            {
                std::cout << "Error creating quest:" << errmsg << std::endl;
            }
            else
            {
                _quests.emplace_back(newRow, std::string(), std::string());
            }
        }

        ImGui::SameLine();
        ImGui::InputText("##", &_search);
        if (ImGui::BeginTable("List of quests", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable))
        {
            for (int i = 0; i < _quests.size(); i++)
            {
                // Filter quests
                if (_search != "")
                {
                    if (isNumber(_search) && std::to_string(_quests[i].id).find(_search) == std::string::npos)
                    {
                        continue;
                    }
                    else if (!isNumber(_search) && _quests[i].name.find(_search) == std::string::npos)
                    {
                        continue;
                    }
                }

                if (renderQuest(_quests[i]))
                {
                    _quests.erase(_quests.begin() + i);
                    i--;
                }
            }
            ImGui::EndTable();
        }
    }

    ImGui::End();
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
        _quests = _qs.getAllQuests();
    }
}
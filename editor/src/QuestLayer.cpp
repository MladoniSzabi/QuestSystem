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

void QuestLayer::updateSql(const std::string &field, const std::string &value, long id)
{
    char *errmsg = nullptr;
    std::string escapedValue = value;
    size_t pos = escapedValue.find("\"", pos);
    while (pos != std::string::npos)
    {
        escapedValue.replace(pos, 1, "\\\"");
        pos = escapedValue.find("\"", pos + 1);
    }
    std::string sql = "UPDATE Quest SET " + field + " = \"" + escapedValue + "\" WHERE Id=" + std::to_string(id);
    int rc = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc)
    {
        std::cout << "Error updating " << field << ": " << errmsg << " | " << sql << std::endl;
    }
}

static int createQuestCallback(void *ptr, int rowCount, char **values, char **rowNames)
{
    long *newRow = (long *)newRow;
    *newRow = atol(values[0]);
    std::cout << "Creating new Quest: " << *newRow << std::endl;
    return 0;
}

void addSoftReturnsToText(std::string &str, float multilineWidth)
{

    float textSize = 0;
    std::string tmpStr = "";
    std::string finalStr = "";
    int curChr = 0;
    while (curChr < str.size())
    {

        if (str[curChr] == '\n')
        {
            finalStr += tmpStr + "\n";
            tmpStr = "";
        }

        tmpStr += str[curChr];
        textSize = ImGui::CalcTextSize(tmpStr.c_str()).x;

        if (textSize > multilineWidth)
        {
            int lastSpace = tmpStr.size() - 1;
            while (tmpStr[lastSpace] != ' ' and lastSpace > 0)
                lastSpace--;
            if (lastSpace == 0)
                lastSpace = tmpStr.size() - 2;
            finalStr += tmpStr.substr(0, lastSpace + 1) + "\r\n";
            if (lastSpace + 1 > tmpStr.size())
                tmpStr = "";
            else
                tmpStr = tmpStr.substr(lastSpace + 1);
        }
        curChr++;
    }
    if (tmpStr.size() > 0)
        finalStr += tmpStr;

    str = finalStr;
};

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
        ImGui::InputText("##Search bar", &_search);
        // ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
        ImGui::Columns(2);
        if (ImGui::BeginListBox("##quests"))
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

                if (ImGui::Selectable(_quests[i].name.c_str(), _selectedQuest == i, 0))
                {
                    _selectedQuest = i;
                }
            }
            ImGui::EndListBox();
        }
        ImGui::NextColumn();
        if (_selectedQuest != -1)
        {
            std::string popupName = "Confirm delete quest: " + std::to_string(_quests[_selectedQuest].id);
            if(ImGui::Button("Delete"))
            {
                ImGui::OpenPopup(popupName.c_str());
            }

            if (ImGui::BeginPopup(popupName.c_str()))
            {
                ImGui::Text("Are you sure you want to delete quest %ld: %s", _quests[_selectedQuest].id, _quests[_selectedQuest].name.c_str());
                ImGui::Spacing();

                if (ImGui::Button("Yes"))
                {
                    char *errmsg = nullptr;
                    std::string sql = "DELETE FROM Quest WHERE Id=" + std::to_string(_quests[_selectedQuest].id);
                    int rc = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
                    if (rc)
                    {
                        std::cout << "There was an error deleting this quest: " << errmsg << std::endl;
                    } else {
                        _quests.erase(_quests.begin() + _selectedQuest);
                        _selectedQuest = -1;
                        ImGui::EndPopup();
                        ImGui::End();
                        return;
                    }
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();
                if (ImGui::Button("No"))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }

            ImGui::BeginGroup();
            ImGui::Text("Name: ");
            ImGui::SameLine();
            if (ImGui::InputText("##name", &_quests[_selectedQuest].name))
            {
                updateSql("name", _quests[_selectedQuest].name, _quests[_selectedQuest].id);
            }
            ImGui::EndGroup();

            ImGui::BeginGroup();
            ImGui::Text("Description: ");
            ImGui::SameLine();
            if (ImGui::InputTextMultiline("##description", &_quests[_selectedQuest].description, ImVec2(0, 0), ImGuiInputTextFlags_WordWrapping))
            {
                updateSql("description", _quests[_selectedQuest].description, _quests[_selectedQuest].id);
            }
            ImGui::EndGroup();

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
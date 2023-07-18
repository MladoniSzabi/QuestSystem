#include "StageLayer.hpp"

#include <iostream>

#include <imgui/imgui_stdlib.h>

#include "util.hpp"

StageLayer::StageLayer(std::reference_wrapper<sqlite3 *> db, std::reference_wrapper<QuestSystem> qs) : _db(db), _qs(qs)
{
}

static int createStageCallback(void *ptr, int rowCount, char **values, char **rowNames)
{
    long *newRow = (long *)newRow;
    *newRow = atol(values[0]);
    return 0;
}

void StageLayer::draw()
{
    if (_db.get() == nullptr)
    {
        ImGui::Text("Open a database first");
        return;
    }

    if (_selectedQuestId == -1)
    {
        ImGui::Text("Select a quest first");
        return;
    }

    if (ImGui::Button("Add Stage"))
    {
        char *errmsg = nullptr;
        long newRow = 0;
        std::string sql = "INSERT INTO Stage(QuestId, Description, Level) Values(" + std::to_string(_selectedQuestId) + ", '', 0) returning Id";
        if (sqlite3_exec(
                _db, sql.c_str(), createStageCallback, (void *)&newRow, &errmsg))
        {
            std::cout << "Error creating stage:" << errmsg << std::endl;
        }
        else
        {
            _stages.emplace_back(newRow, _selectedQuestId, std::string(), 0);
        }
    }

    ImGui::SameLine();
    ImGui::Text("Search: ");
    ImGui::SameLine();
    ImGui::InputText("##Search bar", &_search);
    ImGui::Columns(2);
    if (ImGui::BeginListBox("##stages"))
    {
        for (int i = 0; i < _stages.size(); i++)
        {
            // Filter stages
            if (_search != "")
            {
                if (isNumber(_search) && std::to_string(_stages[i].id).find(_search) == std::string::npos)
                {
                    continue;
                }
                else if (!isNumber(_search) && _stages[i].description.find(_search) == std::string::npos)
                {
                    continue;
                }
            }

            if (ImGui::Selectable(std::to_string(_stages[i].id).c_str(), _selectedStage == i, 0))
            {
                _selectedStage = i;
            }
        }
        ImGui::EndListBox();
    }
    ImGui::NextColumn();
    if (_selectedStage != -1)
    {
        std::string popupName = "Confirm delete stage: " + std::to_string(_stages[_selectedStage].id);
        if (ImGui::Button("Delete"))
        {
            ImGui::OpenPopup(popupName.c_str());
        }

        if (ImGui::BeginPopup(popupName.c_str()))
        {
            ImGui::Text("Are you sure you want to delete stage %ld", _stages[_selectedStage].id);
            ImGui::Spacing();

            if (ImGui::Button("Yes"))
            {
                char *errmsg = nullptr;
                std::string sql = "DELETE FROM Stage WHERE Id=" + std::to_string(_stages[_selectedStage].id);
                int rc = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
                if (rc)
                {
                    std::cout << "There was an error deleting this stage: " << errmsg << std::endl;
                }
                else
                {
                    _stages.erase(_stages.begin() + _selectedStage);
                    _selectedStage = -1;
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
        ImGui::Text("QuestId: ");
        ImGui::SameLine();
        if (ImGui::InputLong("##questId", &_stages[_selectedStage].questId))
        {
            updateSql(_db, "Stage", "QuestId", std::to_string(_stages[_selectedStage].questId), _stages[_selectedStage].id);
        }
        ImGui::EndGroup();

        ImGui::BeginGroup();
        ImGui::Text("Description: ");
        ImGui::SameLine();
        if (ImGui::InputTextMultiline("##description", &_stages[_selectedStage].description, ImVec2(0, 0), ImGuiInputTextFlags_WordWrapping))
        {
            updateSql(_db, "Quest", "description", _stages[_selectedStage].description, _stages[_selectedStage].id);
        }
        ImGui::EndGroup();

        ImGui::BeginGroup();
        ImGui::Text("Order: ");
        ImGui::SameLine();
        if (ImGui::InputLong("##order", &_stages[_selectedStage].order))
        {
            updateSql(_db, "Stage", "Level", std::to_string(_stages[_selectedStage].order), _stages[_selectedStage].id);
        }
        ImGui::EndGroup();
    }
    ImGui::Columns(1);
}

void StageLayer::handleEvent(const std::string &eventName, void *eventData)
{
    if (eventName == "selectedQuest")
    {
        _selectedQuestId = *((long *)eventData);
        _stages = _qs.get().getStagesForQuest(_selectedQuestId);
    }
    else
    {
        std::cout << "Unhandled event in StageLayer: " << eventName << std::endl;
    }
}
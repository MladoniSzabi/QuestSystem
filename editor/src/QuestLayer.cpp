#include "QuestLayer.hpp"

#include <iostream>
#include <algorithm>

#include <imgui/imgui_stdlib.h>

#include "util.hpp"

QuestLayer::QuestLayer(std::reference_wrapper<sqlite3 *> db, std::reference_wrapper<QuestSystem> qs) : _db(db), _qs(qs)
{
}

void QuestLayer::editRequirement(long requirementId, std::string field, std::string value)
{
    updateSql(_db, "Quest_Requirements", field, value, requirementId);
}

void QuestLayer::deleteRequirement(long requirementId)
{
    char *errmsg = nullptr;
    std::string sql = "DELETE FROM Quest_Requirements WHERE Id=" + std::to_string(requirementId);
    int rc = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc)
    {
        std::cout << "There was an error deleting this requirement: " << errmsg << std::endl;
    }
}

Requirement QuestLayer::createRequirement()
{
    char *errmsg = nullptr;
    long newRow = 0;
    std::string sql = "INSERT INTO Quest_Requirements(Item, QuestId, Operand, Value) Values('requirement'," + std::to_string(_quests[_selectedQuest].id) + ", 0, 0) returning Id";
    if (sqlite3_exec(
            _db, sql.c_str(), createEntryCallback, (void *)&newRow, &errmsg))
    {
        std::cout << "Error creating requirement:" << errmsg << std::endl;
    }
    return Requirement(newRow, "requirement", Operand::EQUAL, 0);
}

void QuestLayer::addEventListener(const std::string &eventName, EventListener *eventListener)
{
    _eventListeners[eventName].push_back(eventListener);
}

void QuestLayer::dispatchEvent(const std::string &eventName, void *eventData)
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

void QuestLayer::handleEvent(const std::string &eventName, void *eventData)
{
    if (eventName == "dbChanged")
    {
        _quests = _qs.get().getAllQuests();
    }
    else
    {
        std::cout << "Unhandled event in QuestLayer: " << eventName << std::endl;
    }
}

void QuestLayer::draw()
{
    if (_db.get() == nullptr)
    {
        ImGui::Text("Open a database first");
        return;
    }

    if (ImGui::Button("Add Quest"))
    {
        char *errmsg = nullptr;
        long newRow = 0;
        if (sqlite3_exec(
                _db, "INSERT INTO Quest(Name, Description) Values('', '') returning Id", createEntryCallback, (void *)&newRow, &errmsg))
        {
            std::cout << "Error creating quest:" << errmsg << std::endl;
        }
        else
        {
            _quests.emplace_back(newRow, std::string(), std::string());
        }
    }

    ImGui::SameLine();
    ImGui::Text("Search: ");
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
                long selectedQuestId = _quests[_selectedQuest].id;
                dispatchEvent("selectedQuest", (void *)&selectedQuestId);
            }
        }
        ImGui::EndListBox();
    }
    ImGui::NextColumn();
    if (_selectedQuest == -1)
    {
        ImGui::Columns(1);
        return;
    }

    std::string popupName = "Confirm delete quest: " + std::to_string(_quests[_selectedQuest].id);
    if (ImGui::Button("Delete"))
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
            // TODO: make sure to delete the stages of this quest as well.
            std::string sql = "DELETE FROM Quest WHERE Id=" + std::to_string(_quests[_selectedQuest].id);
            int rc = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
            if (rc)
            {
                std::cout << "There was an error deleting this quest: " << errmsg << std::endl;
            }
            else
            {
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
        updateSql(_db, "Quest", "name", _quests[_selectedQuest].name, _quests[_selectedQuest].id);
    }
    ImGui::EndGroup();

    ImGui::BeginGroup();
    ImGui::Text("Description: ");
    ImGui::SameLine();
    if (ImGui::InputTextMultiline("##description", &_quests[_selectedQuest].description, ImVec2(0, 0), ImGuiInputTextFlags_WordWrapping))
    {
        updateSql(_db, "Quest", "description", _quests[_selectedQuest].description, _quests[_selectedQuest].id);
    }
    ImGui::EndGroup();

    RequirementsTableLayer _requirementsTable(
        [=](long id, std::string f, std::string v)
        { this->editRequirement(id, f, v); },
        [=](long id)
        { this->deleteRequirement(id); },
        [=]()
        { return this->createRequirement(); },
        std::ref(_quests[_selectedQuest].requirements));
    _requirementsTable.draw();

    ImGui::Columns(1);
}
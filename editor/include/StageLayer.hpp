#ifndef STAGELAYER_HPP_
#define STAGELAYER_HPP_

#include <vector>
#include <functional>

#include "imgui/imgui.h"
#include "sqlite/sqlite3.h"

#include "ImGuiLayer.hpp"
#include "EventListener.hpp"
#include "Stage.hpp"
#include "QuestSystem.hpp"
#include "RequirementsTableLayer.hpp"

class StageLayer : public ImGuiLayer, public EventListener<long>
{

private:
    std::reference_wrapper<sqlite3 *> _db;
    std::reference_wrapper<QuestSystem> _qs;
    std::vector<Stage> _stages;
    long _selectedQuestId = -1;
    long _selectedStage = -1;
    std::string _search;

    void editRequirement(long requirementId, std::string field, std::string value);
    void deleteRequirement(long requirementId);
    Requirement createRequirement();

public:
    StageLayer(std::reference_wrapper<sqlite3 *> db, std::reference_wrapper<QuestSystem> qs);
    void draw() override;
    void handleEvent(const std::string &eventName, const long &eventData) override;
};

#endif // STAGELAYER_HPP_
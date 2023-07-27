#ifndef QUESTLAYER_HPP_
#define QUESTLAYER_HPP_

#include <vector>
#include <functional>

#include <sqlite/sqlite3.h>
#include <imgui/imgui.h>

#include "ImGuiLayer.hpp"
#include "QuestSystem.hpp"
#include "Quest.hpp"
#include "EventListener.hpp"
#include "EventEmitter.hpp"
#include "RequirementsTableLayer.hpp"

class QuestLayer : public ImGuiLayer, public EventListener<sqlite3 *>, public EventEmitter<long>
{
private:
    std::reference_wrapper<sqlite3 *> _db;
    std::reference_wrapper<QuestSystem> _qs;
    std::vector<Quest> _quests;
    std::string _search = "";
    std::unordered_map<std::string, std::vector<EventListener *>> _eventListeners;
    long _selectedQuest = -1;

    void renderQuest(Quest &);

    void editRequirement(long requirementId, std::string field, std::string value);
    void deleteRequirement(long requirementId);
    Requirement createRequirement();

public:
    QuestLayer(std::reference_wrapper<sqlite3 *> db, std::reference_wrapper<QuestSystem> qs);
    ~QuestLayer() {}
    void draw() override;
    void handleEvent(const std::string &eventName, sqlite3 *const &eventData) override;
};

#endif // QUESTLAYER_HPP_
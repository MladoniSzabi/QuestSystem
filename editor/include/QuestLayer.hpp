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

class QuestLayer : public ImGuiLayer, public EventListener, public EventEmitter
{
private:
    std::reference_wrapper<sqlite3 *> _db;
    std::reference_wrapper<QuestSystem> _qs;
    std::vector<Quest> _quests;
    std::string _search = "";
    std::unordered_map<std::string, std::vector<EventListener*>> _eventListeners;
    long _selectedQuest = -1;

    void renderQuest(Quest &);
    void updateSql(const std::string &field, const std::string &value, long id);
    void dispatchEvent(const std::string &eventName, void *eventData);

public:
    QuestLayer(std::reference_wrapper<sqlite3 *> db, std::reference_wrapper<QuestSystem> qs);
    ~QuestLayer() {}
    void draw() override;
    void handleEvent(const std::string &eventName, void *eventData) override;
    void addEventListener(const std::string &eventName, EventListener *eventListener) override;
};

#endif // QUESTLAYER_HPP_
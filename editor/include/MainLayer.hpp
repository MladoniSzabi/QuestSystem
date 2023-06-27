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

class MainLayer : public ImGuiLayer, public EventListener, public EventEmitter
{
private:
    sqlite3 *_db = nullptr;
    ImGui::FileBrowser _fileDialog;
    QuestSystem _qs;
    long _selectedQuest = -1;
    std::unordered_map<std::string, std::shared_ptr<ImGuiLayer>> _childLayers;
    std::unordered_map<std::string, std::vector<EventListener *>> _eventListeners;
    void dispatchEvent(const std::string &eventName, void *eventData);

public:
    MainLayer();
    ~MainLayer();
    void draw() override;

    void addEventListener(const std::string &eventName, EventListener *listener) override;
    void handleEvent(const std::string &eventName, void *eventData) override;
};

#endif // MAINLAYER_HPP_
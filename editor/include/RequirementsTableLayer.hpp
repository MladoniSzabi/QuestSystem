#ifndef REQUIREMENTSTABLELAYER_HPP_
#define REQUIREMENTSTABLELAYER_HPP_

#include <string>
#include <functional>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <sqlite/sqlite3.h>

#include "ImGuiLayer.hpp"
#include "Requirement.hpp"

class RequirementsTableLayer : public ImGuiLayer
{
private:
    std::function<void(long, std::string, std::string)> _editCallback;
    std::function<void(long)> _deleteCallback;
    std::function<Requirement()> _createCallback;

    std::reference_wrapper<Requirements> _requirements;

public:
    RequirementsTableLayer(
        std::function<void(long, std::string, std::string)> editCallback,
        std::function<void(long)> deleteCallback,
        std::function<Requirement()> createCallback,
        std::reference_wrapper<Requirements> requirements);

    // void setCurrentRequirements(std::reference_wrapper<Requirements> requirements);

    void draw() override;
};

#endif // REQUIREMENTSTABLELAYER_HPP_
#include "RequirementsTableLayer.hpp"

RequirementsTableLayer::RequirementsTableLayer(
    std::function<void(long, std::string, std::string)> editCallback,
    std::function<void(long)> deleteCallback,
    std::function<Requirement()> createCallback,
    std::reference_wrapper<Requirements> requirements) : _editCallback(editCallback),
                                                         _deleteCallback(deleteCallback),
                                                         _createCallback(createCallback),
                                                         _requirements(requirements)
{
}

void RequirementsTableLayer::draw()
{
    ImGui::BeginGroup();
    ImGui::Text("Requirements: ");

    if (ImGui::Button("Add"))
    {
        _requirements.get().addRequirement("requirement", _createCallback());
    }

    if (ImGui::BeginTable("##requirements", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable))
    {
        ImGui::TableNextColumn();
        ImGui::Text("Name");
        ImGui::TableNextColumn();
        ImGui::Text("Operand");
        ImGui::TableNextColumn();
        ImGui::Text("Value");
        ImGui::TableNextColumn();
        // Delete button

        for (const auto &req_pair : _requirements.get().requirements)
        {
            ImGui::PushID(req_pair.second.id);
            std::string name = req_pair.first;
            ImGui::TableNextColumn();
            if (ImGui::InputText("##req_name", &name))
            {
                _requirements.get().requirements[name] = _requirements.get().requirements[req_pair.first];
                _requirements.get().requirements.erase(req_pair.first);
                _editCallback(req_pair.second.id, "Item", name);
                break; // TODO: Stops the editor from crashing but results in a visual artifact. Not sure how to update a key inside a loop.
            }
            ImGui::TableNextColumn();
            const char *operands[] = {"=", "<", "<=", ">", ">=", "!="};
            int selected = (int)req_pair.second.operand;
            if (ImGui::Combo("##req_operand", &selected, operands, 6)) // TODO: Not sure why but using IM_ARRAYSIZE like in the demo doesn't work
            {
                _requirements.get().requirements[name].operand = (Operand)selected;
                _editCallback(req_pair.second.id, "Operand", std::to_string(selected));
            }
            ImGui::TableNextColumn();
            if (ImGui::InputDouble("##req_value", &_requirements.get().requirements[name].value))
            {
                _editCallback(req_pair.second.id, "Value", std::to_string(req_pair.second.value));
            }
            ImGui::TableNextColumn();

            std::string popupName = "Delete Stage Requirement: " + std::to_string(req_pair.second.id);
            if (ImGui::Button("Delete"))
            {
                ImGui::OpenPopup(popupName.c_str());
            }
            if (ImGui::BeginPopup(popupName.c_str()))
            {
                ImGui::Text("Are you sure you want to delete this requirement?");
                ImGui::Spacing();

                if (ImGui::Button("Yes"))
                {
                    _deleteCallback(req_pair.second.id);
                    _requirements.get().requirements.erase(name);
                    ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                    ImGui::PopID();
                    break;
                }

                ImGui::SameLine();
                if (ImGui::Button("No"))
                {
                    ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
                    ImGui::PopID();
                    break;
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::EndGroup();
}
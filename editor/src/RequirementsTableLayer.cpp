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
        _requirements.get().addRequirement(_createCallback());
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

        for (int i = 0; i < _requirements.get().requirements.size(); i++)
        {
            Requirement &req = _requirements.get().requirements[i];
            ImGui::PushID(req.id);
            std::string name = req.name;
            ImGui::TableNextColumn();
            if (ImGui::InputText("##req_name", &req.name))
            {
                // break; // TODO: Stops the editor from crashing but results in a visual artifact. Not sure how to update a key inside a loop.
                _editCallback(req.id, "Item", req.name);
            }
            ImGui::TableNextColumn();
            const char *operands[] = {"=", "<", "<=", ">", ">=", "!="};
            int selected = (int)req.operand;
            if (ImGui::Combo("##req_operand", &selected, operands, 6)) // TODO: Not sure why but using IM_ARRAYSIZE like in the demo doesn't work
            {
                req.operand = (Operand)selected;
                _editCallback(req.id, "Operand", std::to_string(selected));
            }
            ImGui::TableNextColumn();
            double value = req.value;
            if (ImGui::InputDouble("##req_value", &value))
            {
                req.value = value;
                _editCallback(req.id, "Value", std::to_string(req.value));
            }
            ImGui::TableNextColumn();

            std::string popupName = "Delete Stage Requirement: " + std::to_string(req.id);
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
                    _deleteCallback(req.id);
                    _requirements.get().requirements.erase(_requirements.get().requirements.begin() + i);
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
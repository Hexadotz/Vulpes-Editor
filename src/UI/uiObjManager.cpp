#include "uiObjManager.h"
#include <filesystem>

using json = nlohmann::json;
std::vector<EntityData> objectManagerUi::entity_list;
std::vector<std::string> objectManagerUi::scene_entities;


// takes the object's name as a paramater and add it to the list
void objectManagerUi::add_object(const char* label) {
    std::cout << "LOL\n";
    scene_entities.push_back(label);
}

ImVec2 objectManagerUi::get_panel_size() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float screen_width = viewport->Size.x;
    float screen_height = viewport->Size.y;

    return ImVec2(screen_width * 0.25f, screen_height * 0.5f);
}

void objectManagerUi::load_entity_info()
{
    entity_list.clear();

    //std::cout << "Current path: " << std::filesystem::current_path();

    std::fstream file("../../../../assets/entityList.json");
    if (!file.is_open())
    {
        std::cout << "Error: could not open file!\n";
        return;
    }

    std::cout << "file opened\n";
    
    json data = json::parse(file);

    for (const auto& entity : data["entities"])
    {
        EntityData entity_data;
        entity_data.id = entity.value("id", "error");
        entity_data.display_name = entity.value("display_name", "error");
        entity_data.description = entity.value("description", "penis");

        entity_list.push_back(entity_data);
    }


    file.close();
}

// user interface drawing here
void objectManagerUi::draw_Ui() {
    objectManagerUi::add_windowUi();
    objectManagerUi::property_panelUi();
    objectManagerUi::scene_hierarchyUi();
}

void objectManagerUi::scene_hierarchyUi() {
    ImGui::SetNextWindowSize(get_panel_size());
    ImGui::SetNextWindowPos(ImVec2(0, 19));
    ImGui::Begin("Scene Hierarchy", nullptr, panel_flags);
    if (ImGui::Button("Add"))
    {
        objectManagerUi::draw_add_window = true;
    }
    ImGui::Separator();
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (ImGui::TreeNodeEx("Scene", base_flags))
    {
        for (int i = 0; i < scene_entities.size(); i++)
        {
            std::string item_name = scene_entities.at(i) + "##entity_" + std::to_string(i);
            bool item_open = ImGui::TreeNodeEx(item_name.c_str(), base_flags);
            if (ImGui::BeginPopupContextItem())
            {
                
                if (ImGui::Button("Add child")) {
                    objectManagerUi::add_windowUi();
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetItemTooltip("test");
            }
            if (item_open)
            {
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
    ImGui::End();
}

void objectManagerUi::vector2_widget(sf::Vector2f& value)
{
    if (ImGui::BeginTable("position_table", 3)) {

        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, get_panel_size().y);
        ImGui::TableSetupColumn("Axis", ImGuiTableColumnFlags_WidthFixed, 20.0f);

        float slider_width = 100.0f;

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Position:");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(slider_width);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.3f, 0.1f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.5f, 0.2f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.4f, 0.15f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));

        ImGui::DragFloat("##", &value.x, 0.005f);
        ImGui::PopStyleColor(4);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(slider_width);

        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.3f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.5f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.15f, 0.4f, 0.15f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.3f, 1.0f, 0.3f, 1.0f));

        ImGui::DragFloat("##", &value.y, 0.005f);
        ImGui::PopStyleColor(4);

        ImGui::EndTable();
    }
}

void objectManagerUi::rotation_widget(float& value)
{
    if (ImGui::BeginTable("position_table", 2)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthFixed, get_panel_size().y);


        static float min_rotation = -360.0f;
        static float max_rotaion = 360.f;

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Rotation:");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(90.0f);
        ImGui::SliderScalar("##", ImGuiDataType_Float, &value, &min_rotation, &max_rotaion);
        ImGui::SetItemTooltip("Ctrl + Click to input value manually");

        ImGui::EndTable();
    }
}

void objectManagerUi::property_panelUi() {
    // this will be to show the component's properties
    ImGui::SetNextWindowSize(get_panel_size());

    ImGui::SetNextWindowPos(ImVec2(0, 19 + get_panel_size().y));
    ImGui::Begin("Properties", nullptr, panel_flags);
    ImGui::Text("Object: Null");
    ImGui::Separator();

    static sf::Vector2f entity_pos = sf::Vector2f{};
    objectManagerUi::vector2_widget(entity_pos);
    
    static float rotation_amount = 0.0f;
    objectManagerUi::rotation_widget(rotation_amount);


    ImGui::End();
}

void objectManagerUi::add_windowUi() {
    if (objectManagerUi::draw_add_window)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        
        ImVec2 true_center = ImVec2(viewport->GetCenter().x - add_window_size.x / 2, viewport->GetCenter().y - add_window_size.y / 2);
        ImGui::SetNextWindowPos(true_center);
        ImGui::SetNextWindowSize(add_window_size);
        ImGui::Begin("Add component", nullptr, panel_flags);
        
        std::string cur_entity_name;
        std::string cur_entity_description;
        
        static int item_selected_idx = 0;
        int item_highlighted_idx = -1; // Here we store our highlighted data as an index.
        ImGui::Text("Component to add: ");
        ImGui::Separator();
        if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 7 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int n = 0; n < entity_list.size(); n++)
            {
                const EntityData entity = entity_list.at(n);
                const bool is_selected = (item_selected_idx == n);
                
                if (ImGui::Selectable(entity.display_name.c_str(), is_selected)) {
                    item_selected_idx = n;
                    std::cout << n;
                }
                
                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected){
                    ImGui::SetItemDefaultFocus();
                    cur_entity_name = entity.display_name;
                    cur_entity_description = entity.description;
                }
               
            }
            ImGui::EndListBox();
        }
        ImGui::Separator();
        ImGui::Text("Entity Description:\n%s", cur_entity_description.c_str());
        if (ImGui::Button("Add"))
        {
            objectManagerUi::add_object(cur_entity_name.c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            objectManagerUi::draw_add_window = false;
        }
        ImGui::End();
    }
}
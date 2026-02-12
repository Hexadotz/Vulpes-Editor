#include <SFML/Graphics.hpp>
#include <imgui.h>
#include "uiObjManager.h"
#include <iostream>

ImVec2 get_screen_center();

// takes the object's name as a paramater and add it to the list
void objectManagerUi::add_object() {
    std::cout << "LOL";
}

ImVec2 objectManagerUi::get_panel_size() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float screen_width = viewport->Size.x;
    float screen_height = viewport->Size.y;

    return ImVec2(screen_width * 0.25f, screen_height * 0.5f);
}

ImVec2 get_screen_center() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 screen_center = ImVec2(viewport->Size.x / 2, viewport->Size.y / 2);
    return screen_center;
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
    if (ImGui::TreeNode("Scene"))
    {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen;
        if (ImGui::TreeNodeEx("Parent", base_flags))
        {
            if (ImGui::TreeNodeEx("Child 1", base_flags))
            {
                ImGui::Button("Button for Child 1");
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
    ImGui::End();
}

void objectManagerUi::property_panelUi() {
    // this will be to show the component's properties
    ImGui::SetNextWindowSize(get_panel_size());

    ImGui::SetNextWindowPos(ImVec2(0, 19 + get_panel_size().y));
    ImGui::Begin("Properties", nullptr, panel_flags);
    ImGui::Text("Object: Null");
    ImGui::Separator();

    ImGui::End();
}

void objectManagerUi::add_windowUi() {
    if (objectManagerUi::draw_add_window)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 true_center = ImVec2(get_screen_center().x - add_window_size.x / 2, get_screen_center().y - add_window_size.y / 2);
        ImGui::SetNextWindowPos(true_center);
        ImGui::SetNextWindowSize(add_window_size);
        ImGui::Begin("Add component", nullptr, panel_flags);
        const char* items[] = { "Sprite", "Light", "Tilemap", "AudioPlayer"};
        static const char* items_description[] = { "Allows the drawing of 2D images", "Entity that eluminate the area", "Entity that allows for creating levels", "Entity that plays Audio" };
        static int item_selected_idx = 0; // Here we store our selected data as an index.
        std::string cur_item_description_text = "";

        int item_highlighted_idx = -1; // Here we store our highlighted data as an index.
        ImGui::Text("Component to add:");
        ImGui::Separator();
        if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 7 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (item_selected_idx == n);
                if (ImGui::Selectable(items[n], is_selected))
                    item_selected_idx = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected){
                    ImGui::SetItemDefaultFocus();
                    //std::cout << items_description[item_selected_idx];
                    cur_item_description_text = items_description[item_selected_idx];
                }
               
            }
            ImGui::EndListBox();
        }
        ImGui::Separator();
        ImGui::Text("Entity Description:\n %s", cur_item_description_text.c_str());
        if (ImGui::Button("Add"))
        {
            objectManagerUi::add_object();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            objectManagerUi::draw_add_window = false;
        }
        ImGui::End();
    }
}
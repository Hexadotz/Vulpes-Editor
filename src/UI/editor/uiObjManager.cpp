#include "uiObjManager.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>

ImVec2 get_panel_size();

// takes the object's name as a paramater and add it to the list
void objectManagerUi::add_object() {

}

ImVec2 get_panel_size() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float screen_width = viewport->Size.x;
    float screen_height = viewport->Size.y;

    return ImVec2(screen_width * 0.2f, screen_height * 0.5f);
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
        ImGui::SetNextWindowPos(ImVec2(viewport->Size.x / 2, viewport->Size.y / 2));
        ImGui::Begin("Add component", nullptr, panel_flags);
        const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO" };
        static int item_selected_idx = 0; // Here we store our selected data as an index.

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
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
        ImGui::Separator();
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
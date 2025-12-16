#include "uiManager.h"
#include <imgui-SFML.h>
#include <imgui.h>

bool show_confirm_popup = false;
int selected_object = -1;

// functions definitons
void main_menu_Bar_setup();

int uiManager::initialize_Ui(sf::RenderWindow& window) {
    // for Ui theme setup and stuff lol
    if (!ImGui::SFML::Init(window)) //  if a fuck up happen
        return 0;

    // otherwise it's good to go
    return 1;
}

void uiManager::draw_Ui() {
   // for the actual Ui drawing
    main_menu_Bar_setup();
    //uiManager::scene_hierarchy();
}

void uiManager::shutdown_Ui() {
    ImGui::SFML::Shutdown();
}

void uiManager::test_Ui() {
    ImGui::ShowDemoWindow();
    //uiManager::confirm_window("Are you sure you want to quit?");
    main_menu_Bar_setup();
    static bool closable_group = true;
    ImGui::Begin("Test Panel", &closable_group);
    if (ImGui::Button("fuck you"))
    {
        show_confirm_popup = true;
    }
    
    //uiManager::scene_hierarchy();

    static bool check = true;
    ImGui::Checkbox("checkbox", &check);
    ImGui::End();
}

void uiManager::scene_hierarchy() {
    static std::vector <std::pair<std::string, std::vector<std::string>>> objects = {
        {"Player", {"Transform", "Sprite", "Script"}},
        {"Enemy", {"Transform", "Sprite"}},
        {"Camera", {"Transform", "Camera"}},
        {"Light", {"Transform", "Light"}}
    };
    if (ImGui::Begin("Scene Hierarchy")) {

        for (int i = 0; i < objects.size(); i++) {
            auto& [name, components] = objects[i];

            
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                ImGuiTreeNodeFlags_SpanAvailWidth;

            if (selected_object == i) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            bool is_open = ImGui::TreeNodeEx(name.c_str(), flags);

            // Handle selection
            if (ImGui::IsItemClicked()) {
                selected_object = i;
            }

            // Draw components if open
            if (is_open) {
                for (auto& component : components) {
                    ImGui::TreeNodeEx(component.c_str(),
                        ImGuiTreeNodeFlags_Leaf |
                        ImGuiTreeNodeFlags_NoTreePushOnOpen
                    );
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::End();;
}


int uiManager::confirm_window(const char *msg) {
    if (show_confirm_popup)
    {
        ImGui::SetNextWindowSize(ImVec2(200, 100));
        ImGui::OpenPopup("my_file_popup");
    }
    
    int resault = -1;

    if (ImGui::BeginPopup("my_file_popup", ImGuiWindowFlags_MenuBar))
    {
        ImGui::Text(msg);
        ImGui::Separator();
        if (ImGui::Button("Cancel"))
        {
            resault = 0;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Confirm"))
        {   
            resault = 1;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    show_confirm_popup = false;
    return resault;
}

void main_menu_Bar_setup() {
    int x = uiManager::confirm_window("xt");

    if (ImGui::BeginMainMenuBar()) {
        // File Menu stuff
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("New");
            ImGui::MenuItem("Open");
            ImGui::Separator();
            ImGui::MenuItem("Quit", nullptr, &show_confirm_popup);
            ImGui::EndMenu();
        }
        
        // Editor Menu stuff
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
                //
            };
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
                //
            };
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
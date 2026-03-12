#include "uiManager.h"

bool show_confirm_popup = false;
int selected_object = -1;

// functions definitons
void main_menu_Bar_setup();
void test();

int uiManager::initialize_Ui(sf::RenderWindow& window) {
    // for Ui theme setup and stuff lol
    if (!ImGui::SFML::Init(window)) //  if a fuck up happen
        return 0;

    // otherwise it's good to go
    return 1;
}

void uiManager::draw_Ui() {
    main_menu_Bar_setup();
    objectManagerUi::draw_Ui();
    editorToolManagerUI::draw_ui();
    uiManager::debug_panel();
    //test();
    //ImGui::ShowStyleEditor();
}

void uiManager::shutdown_Ui() {
    ImGui::SFML::Shutdown();
}

static void test() {
    ImGui::Begin("Splitter test");

    static float w = 200.0f;
    static float h = 300.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::BeginChild("child1", ImVec2(w, h), true);
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::InvisibleButton("vsplitter", ImVec2(8.0f, h));
    if (ImGui::IsItemActive())
        w += ImGui::GetIO().MouseDelta.x;

    ImGui::SameLine();
    ImGui::BeginChild("child2", ImVec2(0, h), true);
    ImGui::EndChild();
    ImGui::InvisibleButton("hsplitter", ImVec2(-1, 8.0f));
    if (ImGui::IsItemActive())
        h += ImGui::GetIO().MouseDelta.y;
    ImGui::BeginChild("child3", ImVec2(0, 0), true);
    ImGui::EndChild();
    ImGui::PopStyleVar();

    ImGui::End();
}

// used for debugging stuff
void uiManager::debug_panel() {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    //ImGui::SetNextWindowPos(center);
    ImGui::SetNextWindowSize(ImVec2(250, 200));
    ImGui::Begin("Vulpes Debug Panel", nullptr);
    if (ImGui::Button("Test Button"))
    {
        objectManagerUi::load_entity_info();
    }
    ImGui::End();
}

bool uiManager::confirm_window(const char* msg) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    bool resault = false;

    if (show_confirm_popup)
    {
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));;
        ImGui::OpenPopup("Confirm");
    }

    if (ImGui::BeginPopupModal("Confirm", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(msg);
        ImGui::Separator();

        static bool dont_ask_me_next_time = false;
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::Checkbox("Don't ask me next time", &dont_ask_me_next_time);
        ImGui::PopStyleVar();

        if (ImGui::Button("OK", ImVec2(120, 0))) { 
            resault = true;
            
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { 
            resault = false;
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::EndPopup();
        show_confirm_popup = false;
        //std::cout << std::boolalpha << resault;
        return resault;
    }  
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
            if (ImGui::MenuItem("Undo")) {
                //
            };
            if (ImGui::MenuItem("Redo")) {
                //
            };
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

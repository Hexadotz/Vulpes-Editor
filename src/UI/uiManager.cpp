#include "uiManager.h"
#include <imgui-SFML.h>
#include <imgui.h>

void uiManager::initialize() {
    // for Ui theme setup  and stuff lol
}

void uiManager::drawUi() {
   // for the actual Ui drawing
}

void uiManager::testUi() {
    static bool closable_group = true;
    ImGui::Begin("Test 2", &closable_group);
    ImGui::Button("fuck you");
    static bool check = true;
    ImGui::Checkbox("checkbox", &check);
    ImGui::End();
}

#include "uiConsoleManager.h"
#include <imgui_internal.h>
#include <cstdarg>
#include <ctime>
#include <fstream>
#include <iostream>
#include "../obj/entites/SceneManager.hpp"

static std::vector<ConsoleMessage>& get_messages() {
    static std::vector<ConsoleMessage> messages;
    return messages;
}

static std::mutex& get_mutex() {
    static std::mutex mutex;
    return mutex;
}

static bool& get_autoScroll() {
    static bool autoScroll = true;
    return autoScroll;
}

static bool& get_showTimestamps() {
    static bool showTimestamps = false;
    return showTimestamps;
}

static float& get_messageSpacing() {
    static float messageSpacing = 4.0f;
    return messageSpacing;
}

// ===== PUBLIC METHODS =====
void consoleManagerUI::draw_console_panel() {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

    ImVec2 viewport_size = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowSize(ImVec2(viewport_size.x * 0.75f, viewport_size.y * 0.20f));
    ImGui::SetNextWindowPos(ImVec2(viewport_size.x * 0.25f, viewport_size.y * 0.80f + 19));

    ImGui::Begin("Console", nullptr, window_flags);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("ConsoleTabs", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("Console"))
        {
            draw_console_tab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Debugger"))
        {
            draw_debugger_tab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("File Explorer"))
        {
            draw_file_explorer_tab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void consoleManagerUI::draw_console_tab() {
    // Toolbar
    ImGui::BeginChild("ConsoleToolbar", ImVec2(0, 30), true);

    if (ImGui::Button("Clear")) {
        clear();
    }
    ImGui::SameLine();

    bool& autoScroll = get_autoScroll();
    ImGui::Checkbox("Auto-scroll", &autoScroll);
    ImGui::SameLine();

    bool& showTimestamps = get_showTimestamps();
    ImGui::Checkbox("Timestamps", &showTimestamps);
    ImGui::SameLine();

    if (ImGui::Button("Save to File")) {
        save_to_file("console_output.log");
    }
    ImGui::SameLine();

    auto& messages = get_messages();
    ImGui::Text("Messages: %zu", messages.size());

    ImGui::EndChild();

    ImGui::Separator();

    // Console output area
    ImGui::BeginChild("ConsoleOutput", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

    std::lock_guard<std::mutex> lock(get_mutex());

    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(messages.size()), get_messageSpacing());

    while (clipper.Step()) {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            const auto& msg = messages[i];

            ImVec4 color = get_color_for_type(msg.type);
            ImGui::PushStyleColor(ImGuiCol_Text, color);

            std::string displayText;
            if (showTimestamps && msg.timestamp > 0) {
                char timeBuf[32];
                snprintf(timeBuf, sizeof(timeBuf), "[%.2f] ", msg.timestamp);
                displayText = timeBuf + msg.text;
            }
            else {
                displayText = msg.text;
            }

            ImGui::TextUnformatted(displayText.c_str());
            ImGui::PopStyleColor();

            if (ImGui::IsItemClicked()) {
                ImGui::SetClipboardText(msg.text.c_str());
            }

            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("Click to copy");
                ImGui::EndTooltip();
            }
        }
    }

    if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
}

void consoleManagerUI::draw_debugger_tab() {
    ImGui::Text("Debugger Panel");
    ImGui::Separator();

    if (auto* scene = SceneManager::instance().getActiveScene()) {
        ImGui::Text("Active Scene: %s", scene->getName().c_str());
        ImGui::Text("Entity Count: %zu", scene->getEntityCount());
        ImGui::Text("Selected Entity: %s",
            scene->getSelectedEntity() ? scene->getSelectedEntity()->getName().c_str() : "None");
    }

    ImGui::Separator();
    ImGui::Text("Performance:");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
}

void consoleManagerUI::draw_file_explorer_tab() {
    ImGui::Text("File Explorer (Coming soon)");
}

ImVec4 consoleManagerUI::get_color_for_type(ConsoleMessageType type) {
    switch (type) {
    case ConsoleMessageType::Info:    return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    case ConsoleMessageType::Warning: return ImVec4(1.0f, 1.0f, 0.3f, 1.0f);
    case ConsoleMessageType::Error:   return ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    case ConsoleMessageType::Success: return ImVec4(0.3f, 1.0f, 0.3f, 1.0f);
    default:                          return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

void consoleManagerUI::add_message(const std::string& text, ConsoleMessageType type) {
    std::lock_guard<std::mutex> lock(get_mutex());

    ConsoleMessage msg(text, type);

    if (ImGui::GetCurrentContext() != nullptr) {
        msg.timestamp = static_cast<float>(ImGui::GetTime());
    }
    else {
        msg.timestamp = 0.0f;
    }

    get_messages().push_back(msg);

    if (get_messages().size() > 1000) {
        auto& messages = get_messages();
        messages.erase(messages.begin(), messages.begin() + 500);
    }

    std::cout << "[CONSOLE] " << text << std::endl;
}

void consoleManagerUI::info(const std::string& text) {
    add_message(text, ConsoleMessageType::Info);
}

void consoleManagerUI::warning(const std::string& text) {
    add_message(text, ConsoleMessageType::Warning);
}

void consoleManagerUI::error(const std::string& text) {
    add_message(text, ConsoleMessageType::Error);
}

void consoleManagerUI::success(const std::string& text) {
    add_message(text, ConsoleMessageType::Success);
}

void consoleManagerUI::clear() {
    try {
        std::lock_guard<std::mutex> lock(get_mutex());
        get_messages().clear();
    }
    catch (...) {
        std::cout << "[CONSOLE] Clear called during shutdown, ignoring" << std::endl;
    }
}

void consoleManagerUI::save_to_file(const std::string& path) {
    std::lock_guard<std::mutex> lock(get_mutex());

    std::ofstream file(path);
    if (file.is_open()) {
        for (const auto& msg : get_messages()) {
            file << "[" << msg.timestamp << "] " << msg.text << std::endl;
        }
        file.close();
        success("Console saved to: " + path);
    }
    else {
        error("Failed to save console to: " + path);
    }
}

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

void consoleManagerUI::initialize() {
    if (!s_instance) {
        s_instance = new consoleManagerUI();

        Vulpes::EventBus::instance().subscribe(Vulpes::EventType::EntityCreated, s_instance);
        Vulpes::EventBus::instance().subscribe(Vulpes::EventType::EntityDestroyed, s_instance);
        Vulpes::EventBus::instance().subscribe(Vulpes::EventType::EntityRenamed, s_instance);
        Vulpes::EventBus::instance().subscribe(Vulpes::EventType::EntityReparented, s_instance);
        Vulpes::EventBus::instance().subscribe(Vulpes::EventType::EntityDuplicated, s_instance);
        Vulpes::EventBus::instance().subscribe(Vulpes::EventType::SelectionChanged, s_instance);
        Vulpes::EventBus::instance().subscribe(Vulpes::EventType::SceneCreated, s_instance);
        Vulpes::EventBus::instance().subscribe(Vulpes::EventType::SceneActivated, s_instance);
    }
}

void consoleManagerUI::shutdown() {
    if (s_instance) {
        Vulpes::EventBus::instance().unsubscribe(s_instance);
        delete s_instance;
        s_instance = nullptr;
    }
}

void consoleManagerUI::onEvent(const Vulpes::Event& event) {
    switch (event.type) {
    case Vulpes::EventType::EntityCreated: {
        auto& e = static_cast<const Vulpes::EntityCreatedEvent&>(event);
        success("Created " + e.entityType + ": " + e.entityName + " (ID: " + std::to_string(e.entityID) + ")");
        break;
    }
    case Vulpes::EventType::EntityDestroyed: {
        auto& e = static_cast<const Vulpes::EntityDestroyedEvent&>(event);
        warning("Destroyed: " + e.entityName + " (ID: " + std::to_string(e.entityID) + ")");
        break;
    }
    case Vulpes::EventType::EntityRenamed: {
        auto& e = static_cast<const Vulpes::EntityRenamedEvent&>(event);
        info("Renamed: " + e.oldName + " -> " + e.newName + " (ID: " + std::to_string(e.entityID) + ")");
        break;
    }
    case Vulpes::EventType::EntityReparented: {
        auto& e = static_cast<const Vulpes::EntityReparentedEvent&>(event);
        info("Reparented: " + e.entityName + " (ID: " + std::to_string(e.entityID) + ")");
        break;
    }
    case Vulpes::EventType::EntityDuplicated: {
        auto& e = static_cast<const Vulpes::EntityDuplicatedEvent&>(event);
        success("Duplicated: " + e.entityName + " -> ID: " + std::to_string(e.newID));
        break;
    }
    case Vulpes::EventType::SelectionChanged: {
        auto& e = static_cast<const Vulpes::SelectionChangedEvent&>(event);
        if (e.newEntityID != -1) {
            info("Selection changed to ID: " + std::to_string(e.newEntityID));
        }
        break;
    }
    case Vulpes::EventType::SceneCreated: {
        auto& e = static_cast<const Vulpes::SceneCreatedEvent&>(event);
        success("Scene created: " + e.sceneName);
        break;
    }
    case Vulpes::EventType::SceneActivated: {
        auto& e = static_cast<const Vulpes::SceneActivatedEvent&>(event);
        success("Scene activated: " + e.sceneName);
        break;
    }
    default:
        break;
    }
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

    // === Scene Info ===
    if (auto* scene = SceneManager::instance().getActiveScene()) {
        ImGui::Text("Active Scene: %s", scene->getName().c_str());
        ImGui::Text("Entity Count: %zu", scene->getEntityCount());
        ImGui::Text("Selected Entity: %s",
            scene->getSelectedEntity() ? scene->getSelectedEntity()->getName().c_str() : "None");
    }

    ImGui::Separator();

    static std::vector<float> fpsHistory(120, 0.0f);  
    static std::vector<float> frameTimeHistory(120, 0.0f);
    static int historyOffset = 0;

    float currentFPS = ImGui::GetIO().Framerate;
    float currentFrameTime = 1000.0f / currentFPS; 

    fpsHistory[historyOffset] = currentFPS;
    frameTimeHistory[historyOffset] = currentFrameTime;
    historyOffset = (historyOffset + 1) % 120;

    ImGui::Text("Performance:");
    ImGui::Text("FPS: %.1f", currentFPS);
    ImGui::Text("Frame Time: %.3f ms", currentFrameTime);

    ImGui::Spacing();

    ImGui::Text("FPS History (last 120 frames)");
    float fpsPlotData[120];
    for (int i = 0; i < 120; i++) {
        int idx = (historyOffset + i) % 120;
        fpsPlotData[i] = fpsHistory[idx];
    }

    ImGui::PlotLines("##FPSPlot", fpsPlotData, 120, 0,
        nullptr, 0.0f, 120.0f, ImVec2(0, 80));

    // Display min/max/avg
    float fpsMin = fpsPlotData[0], fpsMax = fpsPlotData[0], fpsSum = 0;
    int validFrames = 0;
    for (int i = 0; i < 120; i++) {
        if (fpsPlotData[i] > 0) {
            if (fpsPlotData[i] < fpsMin) fpsMin = fpsPlotData[i];
            if (fpsPlotData[i] > fpsMax) fpsMax = fpsPlotData[i];
            fpsSum += fpsPlotData[i];
            validFrames++;
        }
    }
    float fpsAvg = validFrames > 0 ? fpsSum / validFrames : 0;

    ImGui::Text("Min: %.1f | Max: %.1f | Avg: %.1f", fpsMin, fpsMax, fpsAvg);

    ImGui::Spacing();

    ImGui::Text("Frame Time History (ms)");

    float frameTimePlotData[120];
    for (int i = 0; i < 120; i++) {
        int idx = (historyOffset + i) % 120;
        frameTimePlotData[i] = frameTimeHistory[idx];
    }

    ImGui::PlotLines("##FrameTimePlot", frameTimePlotData, 120, 0,
        nullptr, 0.0f, 33.0f, ImVec2(0, 80)); 


    float ftMin = frameTimePlotData[0], ftMax = frameTimePlotData[0], ftSum = 0;
    validFrames = 0;
    for (int i = 0; i < 120; i++) {
        if (frameTimePlotData[i] > 0) {
            if (frameTimePlotData[i] < ftMin) ftMin = frameTimePlotData[i];
            if (frameTimePlotData[i] > ftMax) ftMax = frameTimePlotData[i];
            ftSum += frameTimePlotData[i];
            validFrames++;
        }
    }
    float ftAvg = validFrames > 0 ? ftSum / validFrames : 0;

    ImGui::Text("Min: %.2f | Max: %.2f | Avg: %.2f ms", ftMin, ftMax, ftAvg);

    ImGui::Separator();

    ImGui::Text("Memory Usage:");
    ImGui::Text("Textures: --");
    ImGui::Text("Sounds: --");
    ImGui::Text("Entities: %zu",
        SceneManager::instance().getActiveScene() ?
        SceneManager::instance().getActiveScene()->getEntityCount() : 0);

    ImGui::Spacing();

    static bool showHistogram = false;
    ImGui::Checkbox("Show as Histogram", &showHistogram);

    if (showHistogram) {
        ImGui::Text("FPS Distribution:");
        ImGui::PlotHistogram("##FPSHistogram", fpsPlotData, 120, 0,
            nullptr, 0.0f, 120.0f, ImVec2(0, 60));
    }

    if (ImGui::Button("Reset Stats")) {
        historyOffset = 0;
        for (int i = 0; i < 120; i++) {
            fpsHistory[i] = 0.0f;
            frameTimeHistory[i] = 0.0f;
        }
    }
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
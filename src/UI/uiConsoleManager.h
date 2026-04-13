
#pragma once
#include <imgui.h>
#include <vector>
#include <string>
#include <mutex>

#define CONSOLE_INFO(msg)    consoleManagerUI::info(msg)
#define CONSOLE_WARNING(msg) consoleManagerUI::warning(msg)
#define CONSOLE_ERROR(msg)   consoleManagerUI::error(msg)
#define CONSOLE_SUCCESS(msg) consoleManagerUI::success(msg)

enum class ConsoleMessageType {
    Info,
    Warning,
    Error,
    Success
};

struct ConsoleMessage {
    std::string text;
    ConsoleMessageType type;
    float timestamp;

    ConsoleMessage(const std::string& msg, ConsoleMessageType t)
        : text(msg), type(t), timestamp(0.0f) {
    }
};

class consoleManagerUI {
public:
    static void draw_console_panel();
    static void draw_console_tab();
    static void draw_debugger_tab();
    static void draw_file_explorer_tab();

    static void add_message(const std::string& text, ConsoleMessageType type = ConsoleMessageType::Info);
    static void clear();
    static void save_to_file(const std::string& path);

    static void info(const std::string& text);
    static void warning(const std::string& text);
    static void error(const std::string& text);
    static void success(const std::string& text);

private:
    static ImVec4 get_color_for_type(ConsoleMessageType type);

    // NO static inline members here anymore!
};
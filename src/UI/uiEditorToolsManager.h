#pragma once
#ifndef UITOOLSMANAGER
#define UITOOLSMANAGER

#include "uiManager.h"
#include "uiObjManager.h"
#include "../application.h"
#include <imgui.h>
#include <iostream>



class editorToolManagerUI {
public:
	enum class TOOLS {
		SELECT_TOOL,
		MOVE_TOOL,
		ROTATE_TOOL,
		SCALE_TOOL
	};
	static void draw_ui();
	static inline enum TOOLS current_tool_selected = TOOLS::SELECT_TOOL;
	static inline const float panel_y_size = 35;
	static inline bool is_snapping = false;

private:
	static const ImGuiWindowFlags panel_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar;
	static inline const ImVec4 color_selected = ImVec4(0.2f, 0.6f, 0.9f, 1.0f);
	static inline const ImVec4 color_hovered = ImVec4(0.3f, 0.7f, 1.0f, 1.0f);
	static inline const ImVec4 color_active = ImVec4(0.1f, 0.5f, 0.8f, 1.0f);

	static void draw_toolbar();
	static void draw_tool_button(const char* label, TOOLS tool_type);
};

#endif // !UITOOLSMANAGER
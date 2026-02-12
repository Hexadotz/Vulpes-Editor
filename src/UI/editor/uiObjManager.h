#pragma once

#ifndef OBJVIEWER_H
#define OBJVIEWER_H
#endif

#include <imgui-SFML.h>
#include <imgui.h>

class objectManagerUi {
public:
	inline static bool draw_add_window = false;
	
	// emit is gay
	static void draw_Ui();
	static void add_object();
	static void add_windowUi();
	static void property_panelUi();
	static void scene_hierarchyUi();
	static ImVec2 get_panel_size();
private:
	static const ImGuiWindowFlags panel_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	static const ImGuiWindowFlags object_add_panel_flags = ImGuiWindowFlags_NoCollapse;
	static inline const ImVec2 add_window_size = ImVec2(350, 250);
};


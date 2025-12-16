#pragma once

#ifndef OBJVIEWER_H
#define OBJVIEWER_H
#endif

#include <imgui-SFML.h>
#include <imgui.h>

class objectManagerUi {
public:
	static const int PANEL_SIZE_X = 300;
	static const int PANEL_SIZE_Y = 250;
	inline static bool draw_add_window = false;

	static const ImGuiWindowFlags panel_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	// emit is gay
	static void draw_Ui();
	static void add_object();
	static void add_windowUi();
	static void property_panelUi();
	static void scene_hierarchyUi();
};


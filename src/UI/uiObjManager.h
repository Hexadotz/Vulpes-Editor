#pragma once

#ifndef OBJVIEWER_H
#define OBJVIEWER_H

#include "uiManager.h"

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <iostream>
#include <fstream>
#include "../libs/json.hpp"

struct EntityData {
	std::string id;
	std::string display_name;
	std::string description;
};

class objectManagerUi {
public:
	static inline bool draw_add_window = false;
	static std::vector<EntityData> entity_list;
	static std::vector<std::string> scene_entities;

	static inline int cur_entity_selected;
	
	// emit is gay
	static void draw_Ui();
	static void add_object(const char* label);
	static ImVec2 get_panel_size();
	static void load_entity_info();

private:
	static const ImGuiWindowFlags panel_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	static const ImGuiWindowFlags object_add_panel_flags = ImGuiWindowFlags_NoCollapse;
	static inline const ImVec2 add_window_size = ImVec2(350, 250);

	static void add_windowUi();
	static void property_panelUi();
	static void scene_hierarchyUi();
	static void vector2_widget(sf::Vector2f& value);
	static void rotation_widget(float& value);

};
#endif


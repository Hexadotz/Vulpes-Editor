#pragma once

#ifndef UIMANAGER_H
#define UIMANAGER_H
#endif

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <memory>

#include "uiObjManager.h"
#include "uiEditorToolsManager.h"
#include "../obj/entites/testEntity.hpp"
#include "uiConsoleManager.h"


class uiManager {
public:
	// variables
	static inline bool draw_grid = true;
	static inline int cell_size = 32;
	// methods
	static int initialize_Ui(sf::RenderWindow& window);
	static bool confirm_window(const char* msg);
	static void draw_Ui();
	static void shutdown_Ui();
private:
	static void debug_panel();
};
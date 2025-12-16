#pragma once
#ifndef UIMANAGER_H
#define UIMANAGER_H

#endif
#include <SFML/Graphics.hpp>
#include <imgui.h>

class uiManager {
public:
	// variables

	// methods
	static int initialize_Ui(sf::RenderWindow& window);
	static int confirm_window(const char* msg);
	static void draw_Ui();
	static void shutdown_Ui();
	static void scene_hierarchy();
	static void test_Ui();

};
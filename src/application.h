#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <iostream>
#include <imgui.h>

#include "UI/uiManager.h"
#include "UI/uiObjManager.h"
#include "editor/snapGrid.h"
#include "editor/editorRender.h"

namespace Engine {
	class Application {
	public:
		static inline sf::View cam_view;
		Application():
			window(sf::VideoMode({ 800u, 600u }), "Vulpes Editor")
		{	
			window.setFramerateLimit(60);
			cam_view = window.getDefaultView();
		}

		bool process();
		static void center_camera() { cam_view.setCenter({ 1209.0f, 930.0f });} //according to sfml the center is at that coordinate... nice
		static void get_cam_position() {
			sf::Vector2f center = cam_view.getCenter();
			std::cout << center.x << "," << center.y << "\n";
		}

		
	private:
		sf::RenderWindow window;
		
		SnapGrid grid;

		void gui_process();
		void render_process();

	};
}

#endif // !APPLICATION_H

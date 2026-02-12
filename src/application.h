#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

namespace Engine {
	class Application {
	public:
		Application():
			window(sf::VideoMode({ 800u, 600u }), "Vulpes Editor")
		{	
			window.setFramerateLimit(60);
		}

		bool process();
	private:
		sf::RenderWindow window;

		void gui_process();
		void render_process();

	};
}

#endif // !APPLICATION_H

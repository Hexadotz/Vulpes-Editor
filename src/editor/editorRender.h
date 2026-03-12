#pragma once

#ifndef EDITORRENDER_H
#define EDITORRENDER_H

#include <SFML/Graphics.hpp>
#include "../application.h"

namespace Editor {
	class View {
	public:
		static inline bool is_panning = false;
		static inline bool was_panning = false;

		//sf::View cam_view;

		static inline sf::Vector2i last_mouse_pos;
		static void process(sf::RenderWindow& window, sf::View& cam_view);
	private:

		//
	};
}

#endif // !EDITORRENDER_H


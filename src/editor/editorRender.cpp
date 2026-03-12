#include "editorRender.h"

void Editor::View::process(sf::RenderWindow& window, sf::View& cam_view) {
	//panning function
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
		is_panning = true;
		sf::Vector2i cur_mouse_pos = sf::Mouse::getPosition(window);

		if (!was_panning)
		{
			was_panning = true;
			last_mouse_pos = cur_mouse_pos;
		}
		else {
			sf::Vector2i move_dir = cur_mouse_pos - last_mouse_pos;
			cam_view.move(sf::Vector2f(move_dir.x, move_dir.y));
			last_mouse_pos = cur_mouse_pos;
		}

	}
	else {
		was_panning = false;
		is_panning = false;
	}
	//zoom function
	


}

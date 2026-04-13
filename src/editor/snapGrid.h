#pragma once
#ifndef SNAPGRID_H
#define SNAPGRID_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>


namespace Engine {
	class SnapGrid {
	public:
		SnapGrid(float def_ell_size = 32.0f) : cell_size(def_ell_size) {
			line_color = sf::Color(100, 100, 100, 150);
			center_color = sf::Color::Green;
		};

		void draw_cells(sf::RenderWindow& window, const sf::View& view) {
			sf::Vector2u cur_window_size = window.getSize();
			
			sf::Vector2f center = view.getCenter();
			sf::Vector2f size = view.getSize();
			
			float left_side = center.x - size.x / 2.0f;
			float right_side = center.x + size.x / 2.0f;
			float top_side = center.y - size.y / 2.0f;
			float bottom_side = center.y + size.y / 2.0f;

			float margin = cell_size * 100.0f;
			float left_draw = left_side - margin;
			float right_draw = right_side + margin;
			float top_draw = top_side - margin;
			float bottom_draw = bottom_side + margin;


			float x_start = std::floor(left_side / cell_size) * cell_size;
			float y_start = std::floor(top_side / cell_size) * cell_size;

			int lines_x = static_cast<int>((right_side - x_start) / cell_size) + 2;
			int lines_y = static_cast<int>((bottom_side - y_start) / cell_size) + 2;
			
			sf::VertexArray grid_lines(sf::PrimitiveType::Lines);
			sf::Color line_color(200, 200, 200, 100);
			sf::Color thick_line_color(200, 200, 200, 225);
			sf::Color x_axis_line_color(252, 3, 65, 255);
			sf::Color y_axis_line_color(3, 252, 128, 255);


			for (float x_axis = -1024; x_axis <= 1024; x_axis += cell_size)
			{
				if (std::abs(x_axis) < 1)
				{
					grid_lines.append(sf::Vertex{ sf::Vector2f(x_start, 1024), x_axis_line_color });
					grid_lines.append(sf::Vertex{ sf::Vector2f(x_start, -1024), x_axis_line_color });
				}
				else {
					bool thick_line = (static_cast<int>(std::round(x_axis / cell_size)) % 2 == 0);
					sf::Color color = thick_line ? thick_line_color : line_color;

					grid_lines.append(sf::Vertex{ sf::Vector2f(x_axis, 1024), color });
					grid_lines.append(sf::Vertex{ sf::Vector2f(x_axis, -1024), color });;

				}
			}
			
			for (float y_axis = -1024; y_axis <= 1024; y_axis += cell_size)
			{
				if (std::abs(y_axis) < 0.01f)
				{
					grid_lines.append(sf::Vertex{ sf::Vector2f(1024, y_start), y_axis_line_color });
					grid_lines.append(sf::Vertex{ sf::Vector2f(-1024, y_start), y_axis_line_color });
				}
				else {
					bool thick_line = (static_cast<int>(std::round(y_axis / cell_size)) % 2 == 0);
					sf::Color color = thick_line ? thick_line_color : line_color;

					grid_lines.append(sf::Vertex{ sf::Vector2f(1024, y_axis), color });
					grid_lines.append(sf::Vertex{ sf::Vector2f(-1024, y_axis), color });;

				}
			}
			//grid_lines.append(sf::Vertex{ sf::Vector2f(1024, y_start), y_axis_line_color });
			//grid_lines.append(sf::Vertex{ sf::Vector2f(-1024, y_start), y_axis_line_color });
			
			window.draw(grid_lines);
		};

		void set_cell_size(float new_cell_size) {
			cell_size = new_cell_size;
		};
		float get_cell_size() {
			return cell_size;
		}

	private:
		float cell_size;
		sf::Color line_color;
		sf::Color center_color;

	};
}

#endif // !SNAPGRID_H

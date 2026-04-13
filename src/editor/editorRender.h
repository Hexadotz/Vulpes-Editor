// editor/editorRender.h
#pragma once

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <iostream>
#include <algorithm>

class Editor {
public:
    class View {
    public:
        static inline bool is_panning = false;
        static inline bool was_panning = false;
        static inline sf::Vector2i last_mouse_pos;

        static void process(sf::RenderWindow& window, sf::View& cam_view);
    };
};
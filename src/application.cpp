#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include "UI/uiManager.h"
#include "application.h"
#include "UI/editor/uiObjManager.h"

bool Engine::Application::process()
{	
	if (!uiManager::initialize_Ui(window))
		return -1;

	window.setFramerateLimit(60);

    sf::Clock deltaClock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            ImGui::SFML::ProcessEvent(window, *event); // for mouse clicking and stuff

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        // refreshing the UI each frame
        ImGui::SFML::Update(window, deltaClock.restart()); 
        //render ui
        gui_process();
        //render render to render the render
        render_process();

        window.clear(sf::Color(132, 136, 132));
        ImGui::SFML::Render(window);
        window.display();
    }

    uiManager::shutdown_Ui();
	return true;
}

void Engine::Application::gui_process()
{
    uiManager::draw_Ui();
}

void Engine::Application::render_process()
{

}


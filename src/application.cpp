#include "application.h"

bool Engine::Application::process()
{	
	if (!uiManager::initialize_Ui(window))
		return -1;

	window.setFramerateLimit(60);

    Engine::Application::center_camera();

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

        window.clear(sf::Color(132, 132, 132));

        // refreshing the UI each frame, i think
        ImGui::SFML::Update(window, deltaClock.restart()); 
        //render ui
        gui_process();
        //render the render to render the renderable stuff
        render_process();

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
    if (uiManager::draw_grid) {
        grid.set_cell_size(uiManager::cell_size);
        grid.draw_cells(window, cam_view);
    }
    Editor::View::process(window, cam_view);
}


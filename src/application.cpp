#include "application.h"

bool Engine::Application::process()
{	
	if (!uiManager::initialize_Ui(window))
		return -1;

	window.setFramerateLimit(60);

    Engine::Application::center_camera();

    // === NEW: Create test entities in the real scene ===
    auto* scene = SceneManager::instance().getActiveScene();
    if (scene) {
        // Add a test entity
        auto testEntity = std::make_unique<TestEntity>("Player", sf::Color::Red);
        testEntity->setPosition(400, 300);
        scene->addEntity(std::move(testEntity));

        auto child = std::make_unique<TestEntity>("Child", sf::Color::Blue);
        child->setPosition(100, 0);
        child->setScale(0.7f, 0.7f);
        scene->addEntity(std::move(child));
    }

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
        sf::Time delta_time = deltaClock.restart();
        SceneManager::instance().update(delta_time.asSeconds());

        window.clear(sf::Color(132, 132, 132));

        // refreshing the UI each frame, i think
        ImGui::SFML::Update(window, delta_time);
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
    window.setView(cam_view);

    

    if (uiManager::draw_grid) {
        grid.set_cell_size(uiManager::cell_size);
        grid.draw_cells(window, cam_view);
    }

    /*if (g_testScene) {
        g_testScene->render(window);
    }*/

    SceneManager::instance().render(window);

    Editor::View::process(window, cam_view);

    window.setView(window.getDefaultView());
}


#include "application.h"

#include "core/event.hpp"
#include "core/eventBus.hpp"

bool Engine::Application::process()
{	
	if (!uiManager::initialize_Ui(window))
		return -1;

	window.setFramerateLimit(60);

    Engine::Application::center_camera();

    auto* scene = SceneManager::instance().getActiveScene();
    if (scene) {
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

// application.cpp
#include "editor/Gizmo.hpp"

void Engine::Application::render_process()
{
    window.setView(cam_view);

    if (uiManager::draw_grid) {
        grid.setCellSize(static_cast<float>(uiManager::cell_size));
        grid.draw(window, cam_view);
    }

    SceneManager::instance().render(window);

    // Gizmo stuff
    static Gizmo::Gizmo s_gizmo;  

    if (auto* scene = SceneManager::instance().getActiveScene()) {
        if (auto* selected = scene->getSelectedEntity()) {
            switch (editorToolManagerUI::current_tool_selected) {
            case editorToolManagerUI::TOOLS::SELECT_TOOL:
                s_gizmo.setOperation(Gizmo::GizmoOperation::None);
                break;
            case editorToolManagerUI::TOOLS::MOVE_TOOL:
                s_gizmo.setOperation(Gizmo::GizmoOperation::Translate);
                break;
            case editorToolManagerUI::TOOLS::ROTATE_TOOL:
                s_gizmo.setOperation(Gizmo::GizmoOperation::Rotate);
                break;
            case editorToolManagerUI::TOOLS::SCALE_TOOL:
                s_gizmo.setOperation(Gizmo::GizmoOperation::Scale);
                break;
            }
            if (s_gizmo.getOperation() != Gizmo::GizmoOperation::None) {
                s_gizmo.render(window, cam_view, selected);
            }
        }
    }

    Editor::View::process(window, cam_view);

    window.setView(window.getDefaultView());
}


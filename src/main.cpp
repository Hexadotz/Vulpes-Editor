#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include "UI/uiManager.h"
#include "UI/editor/uiObjManager.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "Vulpes Editor");

    window.setFramerateLimit(120);// limit frame rate

    if (!uiManager::initialize_Ui(window))
        return -1;

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

        ImGui::SFML::Update(window, deltaClock.restart()); // updating the UI each frame

        uiManager::test_Ui();
        uiManager::draw_Ui();
        objectManagerUi::draw_Ui();

        window.clear(sf::Color(132, 136, 132));
        ImGui::SFML::Render(window);
        window.display();
    }
    uiManager::shutdown_Ui();

    return 0;
}
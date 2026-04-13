// editor/editorRender.cpp
#include "editorRender.h"
#include "../UI/uiObjManager.h"
#include "../obj/entites/SceneManager.hpp"

void Editor::View::process(sf::RenderWindow& window, sf::View& cam_view) {
    static bool wasLeftPressed = false;
    static bool wasRightPressed = false;
    static bool wasDeletePressed = false;
    static bool wasCtrlDPressed = false;

    bool isLeftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    bool isRightPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

    // delete function

    if (!isLeftPressed && wasLeftPressed && !ImGui::GetIO().WantCaptureMouse) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, cam_view);

        if (auto* activeScene = SceneManager::instance().getActiveScene()) {
            Entity2D* hit = activeScene->getEntityAtPosition(worldPos);
            activeScene->selectEntity(hit);
            objectManagerUi::setSelectedEntity(hit);
        }
    }
    wasLeftPressed = isLeftPressed;


    // panning function
    if (isRightPressed) {
        is_panning = true;
        sf::Vector2i cur_mouse_pos = sf::Mouse::getPosition(window);

        if (!was_panning)
        {
            was_panning = true;
            last_mouse_pos = cur_mouse_pos;
        }
        else {
            sf::Vector2i move_dir = cur_mouse_pos - last_mouse_pos;
            cam_view.move(sf::Vector2f(-move_dir.x, -move_dir.y));
            last_mouse_pos = cur_mouse_pos;
        }
    }
    else {
        was_panning = false;
        is_panning = false;
    }
    wasRightPressed = isRightPressed;

    
    // delete key
    bool isDeletePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Delete);
    if (isDeletePressed && !wasDeletePressed && !ImGui::GetIO().WantCaptureKeyboard) {
        if (auto* activeScene = SceneManager::instance().getActiveScene()) {
            if (auto* selected = activeScene->getSelectedEntity()) {
                std::cout << "[Editor] Deleting entity: " << selected->getName() << std::endl;
                activeScene->removeEntity(selected);
                objectManagerUi::setSelectedEntity(nullptr);
            }
        }
    }
    wasDeletePressed = isDeletePressed;

    // duplicate
    bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
    bool isDPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
    bool ctrlDCombo = isCtrlPressed && isDPressed;

    if (ctrlDCombo && !wasCtrlDPressed && !ImGui::GetIO().WantCaptureKeyboard) {
        if (auto* activeScene = SceneManager::instance().getActiveScene()) {
            if (auto* selected = activeScene->getSelectedEntity()) {
                std::cout << "[Editor] Duplicating entity: " << selected->getName() << std::endl;
                Entity2D* duplicated = activeScene->duplicateEntity(selected);
                activeScene->selectEntity(duplicated);
                objectManagerUi::setSelectedEntity(duplicated);
            }
        }
    }
    wasCtrlDPressed = ctrlDCombo;


    // zoom function
    static float zoomLevel = 1.0f;
    float wheelDelta = ImGui::GetIO().MouseWheel;
    if (wheelDelta != 0.0f && !ImGui::GetIO().WantCaptureMouse) {
        zoomLevel *= (1.0f - wheelDelta * 0.1f);
        zoomLevel = std::clamp(zoomLevel, 0.1f, 5.0f);
        cam_view.setSize(window.getDefaultView().getSize() * zoomLevel);
    }
}
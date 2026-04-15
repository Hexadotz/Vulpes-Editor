#include "editorRender.h"
#include "../UI/uiObjManager.h"
#include "../obj/entites/SceneManager.hpp"
#include "Gizmo.hpp"
#include "../UI/uiConsoleManager.h"

void Editor::View::process(sf::RenderWindow& window, sf::View& cam_view) {
    static bool wasLeftPressed = false;
    static bool wasRightPressed = false;
    static bool wasDeletePressed = false;
    static bool wasCtrlDPressed = false;
    static bool wasF2Pressed = false;
    static bool wasGPressed = false;

    static Gizmo::Gizmo s_gizmo;
    static bool gizmoWasDragging = false;

    bool isLeftPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    bool isRightPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos, cam_view);

    auto* scene = SceneManager::instance().getActiveScene();
    Entity2D* selected = scene ? scene->getSelectedEntity() : nullptr;

    if (selected) {
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
    }

    bool handledByGizmo = false;
    bool gizmoActive = false;
    if (selected && s_gizmo.getOperation() != Gizmo::GizmoOperation::None) {
        if (isLeftPressed && !wasLeftPressed) {
            gizmoActive = s_gizmo.onMousePressed(worldPos, selected, cam_view);
            gizmoWasDragging = gizmoActive;
        }
        else if (isLeftPressed && s_gizmo.isDragging()) {
            s_gizmo.onMouseDragged(worldPos, selected);
            gizmoActive = true;
        }
        else if (!isLeftPressed && s_gizmo.isDragging()) {
            s_gizmo.onMouseReleased();
            gizmoActive = true;
        }
    }

    if (!isLeftPressed && wasLeftPressed && !gizmoWasDragging && !ImGui::GetIO().WantCaptureMouse) {
        if (scene) {
            Entity2D* hit = scene->getEntityAtPosition(worldPos);
            scene->selectEntity(hit);
            objectManagerUi::setSelectedEntity(hit);
        }
    }

    if (!isLeftPressed) {
        gizmoWasDragging = false;
    }
    wasLeftPressed = isLeftPressed;

    // panning
    if (isRightPressed) {
        is_panning = true;
        if (!was_panning) {
            was_panning = true;
            last_mouse_pos = mousePos;
        }
        else {
            sf::Vector2i move_dir = mousePos - last_mouse_pos;
            cam_view.move(sf::Vector2f(static_cast<float>(-move_dir.x), static_cast<float>(-move_dir.y)));
            last_mouse_pos = mousePos;
        }
    }
    else {
        was_panning = false;
        is_panning = false;
    }
    wasRightPressed = isRightPressed;

    bool isDeletePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Delete);
    if (isDeletePressed && !wasDeletePressed && !ImGui::GetIO().WantCaptureKeyboard) {
        if (scene && selected) {
            CONSOLE_INFO("[Editor] Deleting: " + selected->getName());
            scene->deselectAll();
            objectManagerUi::setSelectedEntity(nullptr);
            objectManagerUi::queueEntityForDeletion(selected);
        }
    }
    wasDeletePressed = isDeletePressed;

    bool isCtrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
    bool isDPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
    bool ctrlDCombo = isCtrlPressed && isDPressed;

    if (ctrlDCombo && !wasCtrlDPressed && !ImGui::GetIO().WantCaptureKeyboard) {
        if (scene && selected) {
            CONSOLE_INFO("[Editor] Duplicating: " + selected->getName());
            Entity2D* duplicated = scene->duplicateEntity(selected);
            scene->selectEntity(duplicated);
            objectManagerUi::setSelectedEntity(duplicated);
        }
    }
    wasCtrlDPressed = ctrlDCombo;

    bool isF2Pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F2);
    if (isF2Pressed && !wasF2Pressed && !ImGui::GetIO().WantCaptureKeyboard) {
        if (scene && selected) {
            objectManagerUi::openRenameDialog(selected);
        }
    }
    wasF2Pressed = isF2Pressed;

    bool isGPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G);
    if (isGPressed && !wasGPressed && !ImGui::GetIO().WantCaptureKeyboard) {
        uiManager::draw_grid = !uiManager::draw_grid;
        CONSOLE_INFO(uiManager::draw_grid ? "Grid enabled" : "Grid disabled");
    }
    wasGPressed = isGPressed;

    float wheelDelta = ImGui::GetIO().MouseWheel;
    if (wheelDelta != 0.0f && !ImGui::GetIO().WantCaptureMouse) {
        float zoomFactor = 1.0f - wheelDelta * 0.1f;
        cam_view.zoom(zoomFactor);
    }

    if (selected && s_gizmo.getOperation() != Gizmo::GizmoOperation::None) {
        s_gizmo.render(window, cam_view, selected);
    }
}
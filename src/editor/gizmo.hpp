#pragma once
#include <SFML/Graphics.hpp>
#include "../obj/entites/entity2D.hpp"
#include "../UI/uiManager.h"

namespace Gizmo {

    enum class GizmoOperation {
        None,
        Translate,
        Rotate,
        Scale
    };

    enum class GizmoAxis {
        None,
        X,
        Y,
        XY
    };

    class Gizmo {
    public:
        Gizmo();

        void render(sf::RenderWindow& window, const sf::View& view, Entity2D* entity);

        // Mouse handling
        bool onMousePressed(const sf::Vector2f& mouseWorldPos, Entity2D* entity, const sf::View& view);
        void onMouseDragged(const sf::Vector2f& mouseWorldPos, Entity2D* entity);
        void onMouseReleased();

        void update(Entity2D* entity, const sf::Vector2f& mouseWorldPos);

        void setOperation(GizmoOperation op) { m_operation = op; }
        GizmoOperation getOperation() const { return m_operation; }

        bool isDragging() const { return m_dragging; }
        bool isHovering() const { return m_isHovering; }

    private:
        GizmoOperation m_operation = GizmoOperation::Translate;
        GizmoAxis m_activeAxis = GizmoAxis::None;
        GizmoAxis m_hoveredAxis = GizmoAxis::None;

        bool m_dragging = false;
        bool m_isHovering = false;

        sf::Vector2f m_dragStartMouse;
        sf::Vector2f m_entityStartPos;
        float m_entityStartRot = 0.0f;
        sf::Vector2f m_entityStartScale;

        float m_handleSize = 15.0f;
        float m_handleThickness = 3.0f;
        float m_rotationRadius = 60.0f;

        sf::Color m_colorX = sf::Color::Red;
        sf::Color m_colorY = sf::Color::Green;
        sf::Color m_colorCenter = sf::Color::White;
        sf::Color m_colorHover = sf::Color::Yellow;

        void renderTranslateGizmo(sf::RenderWindow& window, const sf::Vector2f& pos, float zoom);
        void renderRotateGizmo(sf::RenderWindow& window, const sf::Vector2f& pos, float zoom);
        void renderScaleGizmo(sf::RenderWindow& window, const sf::Vector2f& pos, float zoom);

        GizmoAxis getHoveredAxis(const sf::Vector2f& mousePos, const sf::Vector2f& gizmoPos, float zoom);
        GizmoAxis getHoveredScaleAxis(const sf::Vector2f& mousePos, const sf::Vector2f& gizmoPos, float zoom);

        float getZoomLevel(const sf::View& view) const;
    };

} // namespace Gizmo
#include "Gizmo.hpp"
#include <cmath>
#include <iostream>

namespace Gizmo {

    Gizmo::Gizmo() = default;

    void Gizmo::render(sf::RenderWindow& window, const sf::View& view, Entity2D* entity) {
        if (!entity || !entity->isSelected()) return;

        sf::Vector2f worldPos = entity->getWorldPosition();
        float zoom = getZoomLevel(view);

        switch (m_operation) {
        case GizmoOperation::Translate:
            renderTranslateGizmo(window, worldPos, zoom);
            break;
        case GizmoOperation::Rotate:
            renderRotateGizmo(window, worldPos, zoom);
            break;
        case GizmoOperation::Scale:
            renderScaleGizmo(window, worldPos, zoom);
            break;
        default:
            break;
        }
    }

    bool Gizmo::onMousePressed(const sf::Vector2f& mouseWorldPos, Entity2D* entity, const sf::View& view) {
        if (!entity || m_operation == GizmoOperation::None) return false;

        sf::Vector2f gizmoPos = entity->getWorldPosition();
        float zoom = getZoomLevel(view);

        if (m_operation == GizmoOperation::Translate) {
            m_hoveredAxis = getHoveredAxis(mouseWorldPos, gizmoPos, zoom);
        }
        else if (m_operation == GizmoOperation::Scale) {
            m_hoveredAxis = getHoveredScaleAxis(mouseWorldPos, gizmoPos, zoom);
        }
        else if (m_operation == GizmoOperation::Rotate) {
           
            float radius = m_rotationRadius * zoom;
            float dist = std::sqrt(std::pow(mouseWorldPos.x - gizmoPos.x, 2) + std::pow(mouseWorldPos.y - gizmoPos.y, 2));
            if (std::abs(dist - radius) < m_handleSize * zoom * 2.0f) {
                m_hoveredAxis = GizmoAxis::X;  
            }
            else {
                m_hoveredAxis = GizmoAxis::None;
            }
        }

        if (m_hoveredAxis != GizmoAxis::None) {
            m_dragging = true;
            m_activeAxis = m_hoveredAxis;
            m_dragStartMouse = mouseWorldPos;
            m_entityStartPos = entity->getPosition();
            m_entityStartRot = entity->getRotation().asDegrees();
            m_entityStartScale = entity->getScale();

            CONSOLE_INFO("[Gizmo] Started dragging on axis: " + static_cast<int>(m_activeAxis));
            return true;
        }

        return false;
    }

    void Gizmo::onMouseDragged(const sf::Vector2f& mouseWorldPos, Entity2D* entity) {
        if (!m_dragging || !entity) return;

        sf::Vector2f delta = mouseWorldPos - m_dragStartMouse;

        switch (m_operation) {
        case GizmoOperation::Translate: {
            sf::Vector2f newPos = m_entityStartPos;
            if (m_activeAxis == GizmoAxis::X) {
                newPos.x += delta.x;
            }
            else if (m_activeAxis == GizmoAxis::Y) {
                newPos.y += delta.y;
            }
            else {
                newPos += delta;
            }

            if (uiManager::snap_to_grid) {
                float cellSize = static_cast<float>(uiManager::cell_size);

                if (m_activeAxis == GizmoAxis::X) {
                    newPos.x = std::round(newPos.x / cellSize) * cellSize;
                }
                else if (m_activeAxis == GizmoAxis::Y) {
                    newPos.y = std::round(newPos.y / cellSize) * cellSize;
                }
                else {
                    newPos.x = std::round(newPos.x / cellSize) * cellSize;
                    newPos.y = std::round(newPos.y / cellSize) * cellSize;
                }
            }

            entity->setPosition(newPos);
            break;
        }
        case GizmoOperation::Rotate: {
            sf::Vector2f gizmoPos = entity->getWorldPosition();
            sf::Vector2f toStart = m_dragStartMouse - gizmoPos;
            sf::Vector2f toCurrent = mouseWorldPos - gizmoPos;

            float startAngle = std::atan2(toStart.y, toStart.x);
            float currentAngle = std::atan2(toCurrent.y, toCurrent.x);
            float angleDelta = currentAngle - startAngle;
            float newRot = m_entityStartRot + angleDelta * 180.0f / 3.14159f;

            entity->setRotation(sf::degrees(newRot));
            break;
        }
        case GizmoOperation::Scale: {
            sf::Vector2f newScale = m_entityStartScale;

            if (m_activeAxis == GizmoAxis::X) {
                newScale.x = m_entityStartScale.x * (1.0f + delta.x / 100.0f);
            }
            else if (m_activeAxis == GizmoAxis::Y) {
                newScale.y = m_entityStartScale.y * (1.0f + delta.y / 100.0f);
            }
            else if (m_activeAxis == GizmoAxis::XY) {
                float avgDelta = (delta.x + delta.y) / 2.0f;
                newScale.x = m_entityStartScale.x * (1.0f + avgDelta / 100.0f);
                newScale.y = m_entityStartScale.y * (1.0f + avgDelta / 100.0f);
            }

            newScale.x = std::max(0.1f, newScale.x);
            newScale.y = std::max(0.1f, newScale.y);
            entity->setScale(newScale);
            break;
        }
        default: break;
        }
    }

    void Gizmo::onMouseReleased() {
        m_dragging = false;
        m_activeAxis = GizmoAxis::None;
        m_hoveredAxis = GizmoAxis::None;
        CONSOLE_INFO("[Gizmo] Stopped dragging");
    }

    void Gizmo::update(Entity2D* entity, const sf::Vector2f& mouseWorldPos) {
        // Legacy method - now handled by onMouseDragged
        onMouseDragged(mouseWorldPos, entity);
    }

    void Gizmo::renderTranslateGizmo(sf::RenderWindow& window, const sf::Vector2f& pos, float zoom) {
        float length = 80.0f * zoom;
        float handleSize = m_handleSize * zoom;

        // X Axis (Red)
        sf::Color xColor = (m_activeAxis == GizmoAxis::X || m_hoveredAxis == GizmoAxis::X) ? m_colorHover : m_colorX;

        sf::VertexArray xLine(sf::PrimitiveType::Lines, 2);
        xLine[0].position = pos;
        xLine[0].color = xColor;
        xLine[1].position = pos + sf::Vector2f(length, 0);
        xLine[1].color = xColor;
        window.draw(xLine);

        sf::CircleShape xHandle(handleSize);
        xHandle.setOrigin({ handleSize, handleSize });
        xHandle.setPosition(pos + sf::Vector2f(length, 0));
        xHandle.setFillColor(xColor);
        window.draw(xHandle);

        // Y Axis (Green)
        sf::Color yColor = (m_activeAxis == GizmoAxis::Y || m_hoveredAxis == GizmoAxis::Y) ? m_colorHover : m_colorY;

        sf::VertexArray yLine(sf::PrimitiveType::Lines, 2);
        yLine[0].position = pos;
        yLine[0].color = yColor;
        yLine[1].position = pos + sf::Vector2f(0, length);
        yLine[1].color = yColor;
        window.draw(yLine);

        sf::CircleShape yHandle(handleSize);
        yHandle.setOrigin({ handleSize, handleSize });
        yHandle.setPosition(pos + sf::Vector2f(0, length));
        yHandle.setFillColor(yColor);
        window.draw(yHandle);

        // Center box
        sf::Color centerColor = (m_activeAxis == GizmoAxis::None && m_hoveredAxis == GizmoAxis::None) ?
            m_colorCenter : m_colorHover;
        sf::RectangleShape center({ handleSize * 1.5f, handleSize * 1.5f });
        center.setOrigin({ handleSize * 0.75f, handleSize * 0.75f });
        center.setPosition(pos);
        center.setFillColor(centerColor);
        center.setOutlineThickness(1.0f);
        center.setOutlineColor(sf::Color::Black);
        window.draw(center);
    }

    void Gizmo::renderRotateGizmo(sf::RenderWindow& window, const sf::Vector2f& pos, float zoom) {
        float radius = m_rotationRadius * zoom;
        float thickness = m_handleThickness * zoom;

        sf::CircleShape circle(radius);
        circle.setOrigin({ radius, radius });
        circle.setPosition(pos);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOutlineThickness(thickness);
        circle.setOutlineColor(m_dragging ? m_colorHover : m_colorX);
        window.draw(circle);

        float handleSize = m_handleSize * zoom;
        sf::CircleShape handle(handleSize);
        handle.setOrigin({ handleSize, handleSize });
        handle.setPosition(pos + sf::Vector2f(radius, 0));
        handle.setFillColor(m_dragging ? m_colorHover : m_colorX);
        window.draw(handle);
    }

    void Gizmo::renderScaleGizmo(sf::RenderWindow& window, const sf::Vector2f& pos, float zoom) {
        float length = 80.0f * zoom;
        float handleSize = m_handleSize * zoom;

        // X handle
        sf::RectangleShape xHandle({ handleSize, handleSize });
        xHandle.setOrigin({ handleSize / 2.0f, handleSize / 2.0f });
        xHandle.setPosition(pos + sf::Vector2f(length, 0));
        xHandle.setFillColor((m_activeAxis == GizmoAxis::X || m_hoveredAxis == GizmoAxis::X) ? m_colorHover : m_colorX);
        window.draw(xHandle);

        // Y handle
        sf::RectangleShape yHandle({ handleSize, handleSize });
        yHandle.setOrigin({ handleSize / 2.0f, handleSize / 2.0f });
        yHandle.setPosition(pos + sf::Vector2f(0, length));
        yHandle.setFillColor((m_activeAxis == GizmoAxis::Y || m_hoveredAxis == GizmoAxis::Y) ? m_colorHover : m_colorY);
        window.draw(yHandle);

        // XY handle
        sf::RectangleShape xyHandle({ handleSize, handleSize });
        xyHandle.setOrigin({ handleSize / 2.0f, handleSize / 2.0f });
        xyHandle.setPosition(pos + sf::Vector2f(length, length));
        xyHandle.setFillColor((m_activeAxis == GizmoAxis::XY || m_hoveredAxis == GizmoAxis::XY) ? m_colorHover : m_colorCenter);
        window.draw(xyHandle);

        // Lines
        sf::VertexArray lines(sf::PrimitiveType::Lines, 4);
        sf::Color lineColor(100, 100, 100, 100);

        lines[0].position = pos;
        lines[0].color = lineColor;
        lines[1].position = pos + sf::Vector2f(length, 0);
        lines[1].color = lineColor;

        lines[2].position = pos;
        lines[2].color = lineColor;
        lines[3].position = pos + sf::Vector2f(0, length);
        lines[3].color = lineColor;

        window.draw(lines);
    }

    float Gizmo::getZoomLevel(const sf::View& view) const {
        float baseSize = 1280.0f;
        float currentSize = view.getSize().x;
        float zoom = currentSize / baseSize;
        return std::clamp(zoom, 0.5f, 3.0f);
    }

    GizmoAxis Gizmo::getHoveredAxis(const sf::Vector2f& mousePos, const sf::Vector2f& gizmoPos, float zoom) {
        float threshold = m_handleSize * zoom * 2.0f;
        float length = 80.0f * zoom;

        sf::Vector2f localMouse = mousePos - gizmoPos;

        // Check X axis
        if (localMouse.x > 0 && localMouse.x < length + threshold &&
            std::abs(localMouse.y) < threshold) {
            return GizmoAxis::X;
        }

        // Check Y axis
        if (localMouse.y > 0 && localMouse.y < length + threshold &&
            std::abs(localMouse.x) < threshold) {
            return GizmoAxis::Y;
        }

        // Check center box
        if (std::abs(localMouse.x) < threshold && std::abs(localMouse.y) < threshold) {
            return GizmoAxis::XY;  // Center box acts as both axes
        }

        return GizmoAxis::None;
    }

    GizmoAxis Gizmo::getHoveredScaleAxis(const sf::Vector2f& mousePos, const sf::Vector2f& gizmoPos, float zoom) {
        float threshold = m_handleSize * zoom * 2.0f;
        float length = 80.0f * zoom;

        sf::Vector2f localMouse = mousePos - gizmoPos;

        sf::Vector2f xHandlePos(length, 0);
        if (std::abs(localMouse.x - xHandlePos.x) < threshold &&
            std::abs(localMouse.y - xHandlePos.y) < threshold) {
            return GizmoAxis::X;
        }

        sf::Vector2f yHandlePos(0, length);
        if (std::abs(localMouse.x - yHandlePos.x) < threshold &&
            std::abs(localMouse.y - yHandlePos.y) < threshold) {
            return GizmoAxis::Y;
        }

        sf::Vector2f xyHandlePos(length, length);
        if (std::abs(localMouse.x - xyHandlePos.x) < threshold &&
            std::abs(localMouse.y - xyHandlePos.y) < threshold) {
            return GizmoAxis::XY;
        }

        return GizmoAxis::None;
    }

} // namespace Gizmo
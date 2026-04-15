// editor/snapGrid.cpp
#include "snapGrid.h"

namespace Engine {

    SnapGrid::SnapGrid(float cellSize)
        : m_cellSize(cellSize)
        , m_minorLineColor(200, 200, 200, 80)
        , m_majorLineColor(200, 200, 200, 180)
        , m_axisXColor(252, 65, 3, 255)
        , m_axisYColor(65, 252, 128, 255)
    {
        m_vertices.setPrimitiveType(sf::PrimitiveType::Lines);
    }

    void SnapGrid::draw(sf::RenderWindow& window, const sf::View& view) {
        if (!m_visible) return;

        sf::Vector2f center = view.getCenter();
        sf::Vector2f size = view.getSize();

        if (m_cellSize != m_lastCellSize ||
            std::abs(center.x - m_lastCenter.x) > m_cellSize * 2.0f ||
            std::abs(center.y - m_lastCenter.y) > m_cellSize * 2.0f ||
            size.x != m_lastSize.x ||
            size.y != m_lastSize.y ||
            m_needsRebuild)
        {
            rebuildGrid(view);
            m_lastCenter = center;
            m_lastSize = size;
            m_lastCellSize = m_cellSize;
            m_needsRebuild = false;
        }

        window.draw(m_vertices);
    }

    void SnapGrid::rebuildGrid(const sf::View& view) {
        m_vertices.clear();

        sf::Vector2f center = view.getCenter();
        sf::Vector2f size = view.getSize();

        float padding = m_cellSize * 2.0f;
        float left = center.x - size.x / 2.0f - padding;
        float right = center.x + size.x / 2.0f + padding;
        float top = center.y - size.y / 2.0f - padding;
        float bottom = center.y + size.y / 2.0f + padding;

        float startX = std::floor(left / m_cellSize) * m_cellSize;
        float startY = std::floor(top / m_cellSize) * m_cellSize;

        // Vertical lines
        for (float x = startX; x <= right; x += m_cellSize) {
            int cellIndex = static_cast<int>(std::round(x / m_cellSize));

            sf::Color color;
            if (std::abs(x) < 0.5f) {
                color = m_axisXColor;
            }
            else if (cellIndex % 5 == 0) {
                color = m_majorLineColor;
            }
            else {
                color = m_minorLineColor;
            }

            // SFML 3 compatible vertex creation
            sf::Vertex v1;
            v1.position = sf::Vector2f(x, top);
            v1.color = color;

            sf::Vertex v2;
            v2.position = sf::Vector2f(x, bottom);
            v2.color = color;

            m_vertices.append(v1);
            m_vertices.append(v2);
        }

        // Horizontal lines
        for (float y = startY; y <= bottom; y += m_cellSize) {
            int cellIndex = static_cast<int>(std::round(y / m_cellSize));

            sf::Color color;
            if (std::abs(y) < 0.5f) {
                color = m_axisYColor;
            }
            else if (cellIndex % 5 == 0) {
                color = m_majorLineColor;
            }
            else {
                color = m_minorLineColor;
            }

            sf::Vertex v1;
            v1.position = sf::Vector2f(left, y);
            v1.color = color;

            sf::Vertex v2;
            v2.position = sf::Vector2f(right, y);
            v2.color = color;

            m_vertices.append(v1);
            m_vertices.append(v2);
        }
    }

    sf::Vector2f SnapGrid::snap(const sf::Vector2f& position) const {
        if (!m_snapEnabled) return position;

        return sf::Vector2f(
            roundToNearest(position.x, m_cellSize),
            roundToNearest(position.y, m_cellSize)
        );
    }

    void SnapGrid::setCellSize(float size) {
        size = std::max(1.0f, size);
        if (m_cellSize != size) {
            m_cellSize = size;
            m_needsRebuild = true;
        }
    }

    float SnapGrid::roundToNearest(float value, float multiple) const {
        return std::round(value / multiple) * multiple;
    }

} // namespace Engine
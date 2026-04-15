// editor/snapGrid.h (keeping your original name)
#pragma once
#ifndef SNAPGRID_H
#define SNAPGRID_H

#include <SFML/Graphics.hpp>
#include <cmath>

namespace Engine {

    class SnapGrid {
    public:
        SnapGrid(float cellSize = 32.0f);

        void draw(sf::RenderWindow& window, const sf::View& view);

        sf::Vector2f snap(const sf::Vector2f& position) const;

        void setCellSize(float size);
        float getCellSize() const { return m_cellSize; }

        void setSnapEnabled(bool enabled) { m_snapEnabled = enabled; }
        bool isSnapEnabled() const { return m_snapEnabled; }

        void setVisible(bool visible) { m_visible = visible; }
        bool isVisible() const { return m_visible; }

    private:
        float m_cellSize;
        bool m_snapEnabled = true;
        bool m_visible = true;

        sf::Color m_minorLineColor;
        sf::Color m_majorLineColor;
        sf::Color m_axisXColor;
        sf::Color m_axisYColor;

        sf::VertexArray m_vertices;
        sf::Vector2f m_lastCenter;
        sf::Vector2f m_lastSize;
        float m_lastCellSize = 0.0f;
        bool m_needsRebuild = true;

        void rebuildGrid(const sf::View& view);
        float roundToNearest(float value, float multiple) const;
    };

} // namespace Engine

#endif
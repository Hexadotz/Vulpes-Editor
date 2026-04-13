// TestEntityScene.hpp
#pragma once
#include "Entity2D.hpp"
#include <vector>
#include <memory>

// Simple test entity for verification
class TestEntity : public Entity2D {
private:
    sf::RectangleShape m_shape;
    sf::Color m_color;
    float m_size = 50.0f;
    float m_animationTimer = 0.0f;

public:
    TestEntity(const std::string& name = "Test", const sf::Color& color = sf::Color::Red);

    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    void renderEditorUI() override;
    Ptr clone() const override;
    const char* getTypeName() const override { return "TestEntity"; }
    
    sf::FloatRect getLocalBounds() const override;
};

class TestEntityScene {
private:
    std::vector<Entity2D::Ptr> m_rootEntities;
    Entity2D* m_selectedEntity = nullptr;

public:

    ~TestEntityScene();  // Add destructor
    void cleanup();

    TestEntityScene();
    void addChildToEntity(Entity2D* parent, Entity2D::Ptr child);

    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    void handleClick(const sf::Vector2f& worldPos);

    void selectEntity(Entity2D* entity);
    Entity2D* getSelectedEntity() const { return m_selectedEntity; }

    void duplicateSelected();
    void deleteSelected();
    void addTestEntity();

    const std::vector<Entity2D::Ptr>& getEntities() const { return m_rootEntities; }

    void renderHierarchyUI();
    void renderInspectorUI();

private:
    void clearCallbacksRecursive(Entity2D* entity);

    void updateRecursive(Entity2D* entity, float dt);
    void renderRecursive(Entity2D* entity, sf::RenderWindow& window);
    Entity2D* findEntityAtPosition(Entity2D* entity, const sf::Vector2f& worldPos);
    void renderHierarchyNode(Entity2D* entity);
    void setupCallbacksRecursive(Entity2D* entity);
};

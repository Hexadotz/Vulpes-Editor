#pragma once
#include "entity2D.hpp"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <functional>

#include "../../libs/json.hpp"

class Scene {
public:
    using Ptr = std::unique_ptr<Scene>;
    using EntityPtr = Entity2D::Ptr;

    using SceneChangedCallback = std::function<void()>;

private:
    std::string m_name;
    std::vector<EntityPtr> m_rootEntities;
    Entity2D* m_selectedEntity = nullptr;
    bool m_isModified = false;
    SceneChangedCallback m_onSceneChanged;

public:
    Scene(const std::string& name = "New Scene");
    ~Scene();


    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void update(float deltaTime);
    void render(sf::RenderWindow& window);


    Entity2D* addEntity(EntityPtr entity);
    EntityPtr removeEntity(Entity2D* entity);

    void clear();

    Entity2D* duplicateEntity(Entity2D* entity);

    void selectEntity(Entity2D* entity);
    void deselectAll();
    Entity2D* getSelectedEntity() const { return m_selectedEntity; }

    Entity2D* findEntityByName(const std::string& name) const;
    Entity2D* findEntityByID(int id) const;
    Entity2D* getEntityAtPosition(const sf::Vector2f& worldPos);
    std::vector<Entity2D*> getEntitiesInRect(const sf::FloatRect& rect);

    template<typename T>
    std::vector<T*> getEntitiesOfType() {
        std::vector<T*> result;
        for (auto& entity : m_rootEntities) {
            collectEntitiesOfTypeRecursive<T>(entity.get(), result);
        }
        return result;
    }

    void saveToFile(const std::string& path);
    void loadFromFile(const std::string& path);

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; markModified(); }

    const std::vector<EntityPtr>& getRootEntities() const { return m_rootEntities; }
    size_t getEntityCount() const;

    bool isModified() const { return m_isModified; }
    void clearModified() { m_isModified = false; }

    void setSceneChangedCallback(SceneChangedCallback callback) {
        m_onSceneChanged = callback;
    }

    void sortByZOrder();

private:
    void updateRecursive(Entity2D* entity, float deltaTime);
    void renderRecursive(Entity2D* entity, sf::RenderWindow& window);
    Entity2D* findEntityAtPositionRecursive(Entity2D* entity, const sf::Vector2f& worldPos);
    void collectEntitiesRecursive(Entity2D* entity, std::vector<Entity2D*>& out);
    size_t countEntitiesRecursive(Entity2D* entity) const;
    void markModified();

    void rebuildHierarchyFromJson(const nlohmann::json& entitiesJson);

    template<typename T>
    void collectEntitiesOfTypeRecursive(Entity2D* entity, std::vector<T*>& out) {
        if (auto* typed = dynamic_cast<T*>(entity)) {
            out.push_back(typed);
        }
        for (const auto& child : entity->getChildren()) {
            collectEntitiesOfTypeRecursive<T>(child.get(), out);
        }
    }
};

#include "Scene.hpp"
#include <fstream>
#include <iostream>

#include "../../UI/uiManager.h"
using json = nlohmann::json;

Scene::Scene(const std::string& name)
    : m_name(name) {
    std::cout << "[Scene] Created scene: " << m_name << std::endl;
}

Scene::~Scene() {
    std::cout << "[Scene] Destroying scene: " << m_name << std::endl;

    if (m_selectedEntity) {
        m_selectedEntity->setSelected(false);
        m_selectedEntity = nullptr;
    }

    for (auto& entity : m_rootEntities) {
        if (entity) {
            entity->clearCallbacks();
        }
    }

    m_rootEntities.clear();

    std::cout << "[Scene] Destroyed scene: " << m_name << std::endl;
}

void Scene::clear() {
    if (m_selectedEntity) {
        m_selectedEntity->setSelected(false);
        m_selectedEntity = nullptr;
    }

    for (auto& entity : m_rootEntities) {
        if (entity) {
            entity->clearCallbacks();
        }
    }

    m_rootEntities.clear();
    markModified();
    std::cout << "[Scene] Cleared all entities" << std::endl;
}

void Scene::update(float deltaTime) {
    for (auto& entity : m_rootEntities) {
        if (entity) {
            updateRecursive(entity.get(), deltaTime);
        }
    }
}

void Scene::render(sf::RenderWindow& window) {
    // Collect all entities for Z-order sorting
    std::vector<Entity2D*> allEntities;
    for (auto& entity : m_rootEntities) {
        if (entity && entity->isVisible()) {
            collectEntitiesRecursive(entity.get(), allEntities);
        }
    }

    // Sort by Z-order
    std::sort(allEntities.begin(), allEntities.end(),
        [](Entity2D* a, Entity2D* b) {
            return a->getZOrder() < b->getZOrder();
        });

    // Render in order
    for (auto* entity : allEntities) {
        entity->render(window);
    }
}

Entity2D* Scene::addEntity(EntityPtr entity) {
    if (!entity) return nullptr;

    Entity2D* ptr = entity.get();
    m_rootEntities.push_back(std::move(entity));

    std::cout << "[Scene] Added entity: " << ptr->getName()
        << " (ID: " << ptr->getUniqueID() << ")" << std::endl;

    markModified();
    return ptr;
}

// obj/entites/Scene.cpp
Scene::EntityPtr Scene::removeEntity(Entity2D* entity) {
    if (!entity) return nullptr;

    CONSOLE_INFO("[Scene] Attempting to remove: " + entity->getName() + " (ID: " + std::to_string(entity->getUniqueID()) + ")");

    // Check root entities first
    auto it = std::find_if(m_rootEntities.begin(), m_rootEntities.end(),
        [entity](const EntityPtr& ptr) { return ptr.get() == entity; });

    if (it != m_rootEntities.end()) {
        // Clear selection if this entity was selected
        if (m_selectedEntity == entity) {
            m_selectedEntity->setSelected(false);
            m_selectedEntity = nullptr;
        }

        EntityPtr removed = std::move(*it);
        m_rootEntities.erase(it);

        CONSOLE_SUCCESS("[Scene] Removed root entity: " + removed->getName());
        markModified();
        return removed;  // Return the unique_ptr
    }

    // Entity not found in root - check children recursively
    for (auto& rootEntity : m_rootEntities) {
        if (rootEntity) {
            EntityPtr removed = rootEntity->removeChild(entity);
            if (removed) {
                CONSOLE_SUCCESS("[Scene] Removed child entity: " + removed->getName());

                if (m_selectedEntity == entity) {
                    m_selectedEntity->setSelected(false);
                    m_selectedEntity = nullptr;
                }

                markModified();
                return removed;  // Return the unique_ptr
            }
        }
    }

    CONSOLE_WARNING("[Scene] Entity not found for removal: " + entity->getName());
    return nullptr;
}

Entity2D* Scene::duplicateEntity(Entity2D* entity) {
    if (!entity) return nullptr;

    std::cout << "[Scene] Duplicating: " << entity->getName()
        << " (ID: " << entity->getUniqueID() << ")" << std::endl;

    // Clone the entity
    auto clone = entity->clone();
    if (!clone) {
        std::cerr << "[Scene] Clone failed!" << std::endl;
        return nullptr;
    }

    // Set new name
    std::string newName = entity->getName() + "_Copy";
    clone->setName(newName);

    // Offset position
    clone->setPosition(entity->getPosition() + sf::Vector2f(30.0f, 30.0f));

    // Clear any callbacks that might cause issues
    clone->setPropertyChangedCallback(nullptr);

    Entity2D* clonePtr = clone.get();

    // Add to same parent or as root
    Entity2D* parent = entity->getParent();

    if (parent) {
        parent->addChild(std::move(clone));
        std::cout << "[Scene] Added as child of: " << parent->getName() << std::endl;
    }
    else {
        m_rootEntities.push_back(std::move(clone));
        std::cout << "[Scene] Added as root entity" << std::endl;
    }

    markModified();

    return clonePtr;
}

void Scene::selectEntity(Entity2D* entity) {
    if (m_selectedEntity == entity) return;

    if (m_selectedEntity) {
        m_selectedEntity->setSelected(false);
    }

    m_selectedEntity = entity;

    if (m_selectedEntity) {
        m_selectedEntity->setSelected(true);
        std::cout << "[Scene] Selected: " << m_selectedEntity->getName() << std::endl;
    }
}

void Scene::deselectAll() {
    selectEntity(nullptr);
}

Entity2D* Scene::findEntityByName(const std::string& name) const {
    for (const auto& entity : m_rootEntities) {
        if (entity->getName() == name) {
            return entity.get();
        }
        if (Entity2D* found = entity->findChild(name)) {
            return found;
        }
    }
    return nullptr;
}

Entity2D* Scene::findEntityByID(int id) const {
    for (const auto& entity : m_rootEntities) {
        if (entity->getUniqueID() == id) {
            return entity.get();
        }
        if (Entity2D* found = entity->findChildByID(id)) {
            return found;
        }
    }
    return nullptr;
}

Entity2D* Scene::getEntityAtPosition(const sf::Vector2f& worldPos) {
    // Search from top to bottom (reverse order)
    for (auto it = m_rootEntities.rbegin(); it != m_rootEntities.rend(); ++it) {
        if (Entity2D* hit = findEntityAtPositionRecursive(it->get(), worldPos)) {
            return hit;
        }
    }
    return nullptr;
}

std::vector<Entity2D*> Scene::getEntitiesInRect(const sf::FloatRect& rect) {
    std::vector<Entity2D*> result;
    for (auto& entity : m_rootEntities) {
        if (entity->getGlobalBounds().findIntersection(rect)) {
            result.push_back(entity.get());
        }
    }
    return result;
}

size_t Scene::getEntityCount() const {
    size_t count = 0;
    for (const auto& entity : m_rootEntities) {
        count += countEntitiesRecursive(entity.get());
    }
    return count;
}

void Scene::sortByZOrder() {
    std::sort(m_rootEntities.begin(), m_rootEntities.end(),
        [](const EntityPtr& a, const EntityPtr& b) {
            return a->getZOrder() < b->getZOrder();
        });
    markModified();
}

void Scene::saveToFile(const std::string& path) {
    json j;
    j["name"] = m_name;
    j["entities"] = json::array();

    // TODO: Implement entity serialization
    // This requires each entity type to have a serialize() method

    std::ofstream file(path);
    if (file.is_open()) {
        file << j.dump(4);
        std::cout << "[Scene] Saved to: " << path << std::endl;
        clearModified();
    }
}

void Scene::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Scene] Failed to open: " << path << std::endl;
        return;
    }

    json j = json::parse(file);
    m_name = j.value("name", "Loaded Scene");

    clear();

    // TODO: Implement entity deserialization

    std::cout << "[Scene] Loaded from: " << path << std::endl;
    clearModified();
}

// === Private Helpers ===

void Scene::updateRecursive(Entity2D* entity, float deltaTime) {
    if (!entity) return;

    if (entity->isActive()) {
        entity->update(deltaTime);
    }

    for (const auto& child : entity->getChildren()) {
        updateRecursive(child.get(), deltaTime);
    }
}

void Scene::renderRecursive(Entity2D* entity, sf::RenderWindow& window) {
    if (!entity) return;

    if (entity->isVisible()) {
        entity->render(window);
    }

    for (const auto& child : entity->getChildren()) {
        renderRecursive(child.get(), window);
    }
}

void Scene::collectEntitiesRecursive(Entity2D* entity, std::vector<Entity2D*>& out) {
    if (!entity) return;

    out.push_back(entity);

    for (const auto& child : entity->getChildren()) {
        if (child->isVisible()) {
            collectEntitiesRecursive(child.get(), out);
        }
    }
}

Entity2D* Scene::findEntityAtPositionRecursive(Entity2D* entity, const sf::Vector2f& worldPos) {
    if (!entity) return nullptr;

    // Check children first (they render on top)
    for (auto& child : entity->getChildren()) {
        if (Entity2D* hit = findEntityAtPositionRecursive(child.get(), worldPos)) {
            return hit;
        }
    }

    // Then check this entity
    if (entity->containsPoint(worldPos)) {
        return entity;
    }

    return nullptr;
}

size_t Scene::countEntitiesRecursive(Entity2D* entity) const {
    if (!entity) return 0;

    size_t count = 1;
    for (const auto& child : entity->getChildren()) {
        count += countEntitiesRecursive(child.get());
    }
    return count;
}

void Scene::markModified() {
    m_isModified = true;
    if (m_onSceneChanged) {
        m_onSceneChanged();
    }
}
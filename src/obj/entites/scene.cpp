
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

   

    m_rootEntities.clear();

    std::cout << "[Scene] Destroyed scene: " << m_name << std::endl;
}

void Scene::clear() {
    if (m_selectedEntity) {
        m_selectedEntity->setSelected(false);
        m_selectedEntity = nullptr;
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
    std::vector<Entity2D*> allEntities;
    for (auto& entity : m_rootEntities) {
        if (entity && entity->isVisible()) {
            collectEntitiesRecursive(entity.get(), allEntities);
        }
    }

    std::sort(allEntities.begin(), allEntities.end(),
        [](Entity2D* a, Entity2D* b) {
            return a->getZOrder() < b->getZOrder();
        });

    for (auto* entity : allEntities) {
        entity->render(window);
    }
}

Entity2D* Scene::addEntity(EntityPtr entity) {
    if (!entity) return nullptr;

    Entity2D* ptr = entity.get();
    m_rootEntities.push_back(std::move(entity));

    Vulpes::EventBus::instance().post<Vulpes::EntityCreatedEvent>(
        ptr->getUniqueID(), ptr->getName(), ptr->getTypeName()
    );

    markModified();
    return ptr;
}

Scene::EntityPtr Scene::removeEntity(Entity2D* entity) {
    if (!entity) return nullptr;

    Vulpes::EventBus::instance().post<Vulpes::EntityDestroyedEvent>(
        entity->getUniqueID(), entity->getName()
    );

    CONSOLE_INFO("[Scene] Attempting to remove: " + entity->getName() + " (ID: " + std::to_string(entity->getUniqueID()) + ")");

    auto it = std::find_if(m_rootEntities.begin(), m_rootEntities.end(),
        [entity](const EntityPtr& ptr) { return ptr.get() == entity; });

    if (it != m_rootEntities.end()) {
        if (m_selectedEntity == entity) {
            m_selectedEntity->setSelected(false);
            m_selectedEntity = nullptr;
        }

        EntityPtr removed = std::move(*it);
        m_rootEntities.erase(it);

        CONSOLE_SUCCESS("[Scene] Removed root entity: " + removed->getName());
        markModified();
        return removed;  
    }

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
                return removed;  
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

    auto clone = entity->clone();
    if (!clone) {
        std::cerr << "[Scene] Clone failed!" << std::endl;
        return nullptr;
    }

    std::string newName = entity->getName() + "_Copy";
    clone->setName(newName);

    clone->setPosition(entity->getPosition() + sf::Vector2f(30.0f, 30.0f));


    Entity2D* clonePtr = clone.get();

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

    int oldID = m_selectedEntity ? m_selectedEntity->getUniqueID() : -1;

    if (m_selectedEntity) {
        m_selectedEntity->setSelected(false);
    }

    m_selectedEntity = entity;

    if (m_selectedEntity) {
        m_selectedEntity->setSelected(true);
    }

    int newID = m_selectedEntity ? m_selectedEntity->getUniqueID() : -1;

    Vulpes::EventBus::instance().post<Vulpes::SelectionChangedEvent>(oldID, newID);
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
    nlohmann::json j;

    j["name"] = m_name;
    j["version"] = "1.0";
    j["entities"] = nlohmann::json::array();

    for (const auto& entity : m_rootEntities) {
        if (entity) {
            j["entities"].push_back(entity->toJson());
        }
    }

    std::ofstream file(path);
    if (file.is_open()) {
        file << j.dump(4); 
        CONSOLE_SUCCESS("Scene saved to: " + path);
        clearModified();
    }
    else {
        CONSOLE_ERROR("Failed to save scene to: " + path);
    }
}

void Scene::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        CONSOLE_ERROR("Failed to open scene file: " + path);
        return;
    }

    try {
        nlohmann::json j = nlohmann::json::parse(file);

        clear();

        if (j.contains("name")) {
            m_name = j["name"].get<std::string>();
        }

        if (j.contains("entities") && j["entities"].is_array()) {
            for (const auto& entityJson : j["entities"]) {
                if (entityJson.contains("type")) {
                    std::string type = entityJson["type"].get<std::string>();

                    EntityPtr entity;
                    if (type == "TestEntity") {
                        entity = std::make_unique<TestEntity>();
                    }

                    if (entity) {
                        entity->fromJson(entityJson);

                        Vulpes::EventBus::instance().post<Vulpes::EntityCreatedEvent>(
                            entity->getUniqueID(), entity->getName(), type
                        );

                        m_rootEntities.push_back(std::move(entity));
                    }
                }
            }

            rebuildHierarchyFromJson(j["entities"]);
        }

        CONSOLE_SUCCESS("Scene loaded from: " + path);
        clearModified();

    }
    catch (const std::exception& e) {
        CONSOLE_ERROR("Failed to parse scene file: " + std::string(e.what()));
    }
}

void Scene::rebuildHierarchyFromJson(const nlohmann::json& entitiesJson) {
    std::unordered_map<int, Entity2D*> idMap;
    for (const auto& entity : m_rootEntities) {
        if (entity) {
            idMap[entity->getUniqueID()] = entity.get();
        }
    }
}



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
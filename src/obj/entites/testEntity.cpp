#include "testEntity.hpp"
#include <imgui.h>
#include <iostream>
#include "../../libs/json.hpp"

TestEntity::TestEntity(const std::string& name, const sf::Color& color)
    : Entity2D(name), m_color(color) {
    m_shape.setSize({ m_size, m_size });
    m_shape.setFillColor(m_color);
    m_shape.setOrigin({ m_size / 2.0f, m_size / 2.0f });
}

nlohmann::json TestEntity::toJson() const {
    nlohmann::json j = Entity2D::toJson();

    j["size"] = m_size;
    j["color"] = { m_color.r, m_color.g, m_color.b, m_color.a };

    return j;
}

void TestEntity::fromJson(const nlohmann::json& j) {
    Entity2D::fromJson(j);

    if (j.contains("size")) {
        m_size = j["size"].get<float>();
        m_shape.setSize({ m_size, m_size });
        m_shape.setOrigin({ m_size / 2.0f, m_size / 2.0f });
    }

    if (j.contains("color") && j["color"].is_array() && j["color"].size() >= 4) {
        m_color = sf::Color(
            j["color"][0].get<uint8_t>(),
            j["color"][1].get<uint8_t>(),
            j["color"][2].get<uint8_t>(),
            j["color"][3].get<uint8_t>()
        );
        m_shape.setFillColor(m_color);
    }
}

void TestEntity::update(float deltaTime) {
    if (m_isSelected) {
        float pulse = 1.0f + 0.2f * std::sin(m_animationTimer * 5.0f);
        m_shape.setScale({ pulse, pulse });
        m_animationTimer += deltaTime;
    }
    else {
        m_shape.setScale({ 1.0f, 1.0f });
        m_animationTimer = 0.0f;
    }
}

void TestEntity::render(sf::RenderWindow& window) {
    if (!m_isVisible) return;

    if (m_isSelected) {
        m_shape.setOutlineThickness(2.0f);
        m_shape.setOutlineColor(sf::Color::Yellow);
    }
    else {
        m_shape.setOutlineThickness(0.0f);
    }

    window.draw(m_shape, getWorldTransform());

    if (!m_children.empty()) {
        sf::CircleShape dot(3.0f);
        dot.setFillColor(sf::Color::White);
        dot.setOrigin({ 1.5f, 1.5f });
        dot.setPosition({ m_size / 2.0f, -m_size / 2.0f });
        window.draw(dot, getWorldTransform());
    }
}

void TestEntity::renderEditorUI() {
    renderTransformUI();

    if (ImGui::CollapsingHeader("Test Entity")) {
        float color[4] = {
            m_color.r / 255.0f,
            m_color.g / 255.0f,
            m_color.b / 255.0f,
            m_color.a / 255.0f
        };
        if (ImGui::ColorEdit4("Color", color)) {
            m_color = sf::Color(
                static_cast<uint8_t>(color[0] * 255),
                static_cast<uint8_t>(color[1] * 255),
                static_cast<uint8_t>(color[2] * 255),
                static_cast<uint8_t>(color[3] * 255)
            );
            m_shape.setFillColor(m_color);
        }

        if (ImGui::DragFloat("Size", &m_size, 1.0f, 10.0f, 200.0f)) {
            m_shape.setSize({ m_size, m_size });
            m_shape.setOrigin({ m_size / 2.0f, m_size / 2.0f });
        }
    }
}

Entity2D::Ptr TestEntity::clone() const {
    auto clone = std::make_unique<TestEntity>(m_name, m_color);

    clone->setPosition(getPosition());
    clone->setRotation(getRotation());
    clone->setScale(getScale());
    clone->setOrigin(getOrigin());

    clone->setVisible(m_isVisible);
    clone->setActive(m_isActive);
    clone->setZOrder(m_zOrder);

    clone->m_size = m_size;
    clone->m_shape.setSize({ m_size, m_size });
    clone->m_shape.setOrigin({ m_size / 2.0f, m_size / 2.0f });
    clone->m_shape.setFillColor(m_color);


    return clone;
}
sf::FloatRect TestEntity::getLocalBounds() const {
    return m_shape.getLocalBounds();
}

TestEntityScene::TestEntityScene() {
    std::cout << "=== Entity2D Test Initialized ===" << std::endl;

    // Create root entity
    auto root = std::make_unique<TestEntity>("Root", sf::Color::Red);
    root->setPosition(400, 300);

    // Create children
    auto child1 = std::make_unique<TestEntity>("Child1", sf::Color::Blue);
    child1->setPosition(100, 0);
    child1->setScale(0.7f, 0.7f);

    auto child2 = std::make_unique<TestEntity>("Child2", sf::Color::Green);
    child2->setPosition(-50, 80);
    child2->setScale(0.5f, 0.5f);
    child2->setRotation(sf::degrees(45));

    auto grandchild = std::make_unique<TestEntity>("Grandchild", sf::Color::Yellow);
    grandchild->setPosition(60, 0);
    grandchild->setScale(0.8f, 0.8f);

    // Build hierarchy
    child2->addChild(std::move(grandchild));
    root->addChild(std::move(child1));
    root->addChild(std::move(child2));

    setupCallbacksRecursive(root.get());
    m_rootEntities.push_back(std::move(root));

    // Add independent entity
    auto independent = std::make_unique<TestEntity>("Independent", sf::Color::Magenta);
    independent->setPosition(800, 400);
    independent->setRotation(sf::degrees(30));
    setupCallbacksRecursive(independent.get());
    m_rootEntities.push_back(std::move(independent));

    selectEntity(m_rootEntities[0].get());
}

void TestEntityScene::update(float deltaTime) {
    for (auto& entity : m_rootEntities) {
        updateRecursive(entity.get(), deltaTime);
    }
}

void TestEntityScene::render(sf::RenderWindow& window) {
    for (auto& entity : m_rootEntities) {
        renderRecursive(entity.get(), window);
    }
}

void TestEntityScene::handleClick(const sf::Vector2f& worldPos) {
    for (auto it = m_rootEntities.rbegin(); it != m_rootEntities.rend(); ++it) {
        Entity2D* hit = findEntityAtPosition(it->get(), worldPos);
        if (hit) {
            selectEntity(hit);
            return;
        }
    }
    selectEntity(nullptr);
}

void TestEntityScene::selectEntity(Entity2D* entity) {
    if (m_selectedEntity) {
        m_selectedEntity->setSelected(false);
    }
    m_selectedEntity = entity;
    if (m_selectedEntity) {
        m_selectedEntity->setSelected(true);
        std::cout << "Selected: " << m_selectedEntity->getName() << std::endl;
    }
}

void TestEntityScene::duplicateSelected() {
    if (m_selectedEntity) {
        auto clone = m_selectedEntity->clone();
        clone->setPosition(m_selectedEntity->getPosition() + sf::Vector2f(30.0f, 30.0f));
        Entity2D* newEntity = clone.get();

        // Find where to insert (same level as selected)
        // For simplicity, add to root if selected is root
        m_rootEntities.push_back(std::move(clone));
        selectEntity(newEntity);
        std::cout << "Duplicated: " << newEntity->getName() << std::endl;
    }
}

// TestEntityScene.cpp

TestEntityScene::~TestEntityScene() {
    cleanup();
}

void TestEntityScene::cleanup() {
    // Clear callbacks from all entities before destruction
    for (auto& entity : m_rootEntities) {
        if (entity) {
            clearCallbacksRecursive(entity.get());
        }
    }
    m_rootEntities.clear();
    m_selectedEntity = nullptr;
}

void TestEntityScene::clearCallbacksRecursive(Entity2D* entity) {
    if (!entity) return;


    // Clear children's callbacks
    for (const auto& child : entity->getChildren()) {
        clearCallbacksRecursive(child.get());
    }
}

void TestEntityScene::deleteSelected() {
    if (!m_selectedEntity) return;

    auto it = std::find_if(m_rootEntities.begin(), m_rootEntities.end(),
        [this](const Entity2D::Ptr& ptr) { return ptr.get() == m_selectedEntity; });

    if (it != m_rootEntities.end()) {
        std::cout << "Deleted: " << m_selectedEntity->getName() << std::endl;

        Entity2D* toDelete = m_selectedEntity;
        m_selectedEntity = nullptr;

        m_rootEntities.erase(it);
        selectEntity(m_rootEntities.empty() ? nullptr : m_rootEntities[0].get());
    }
}

void TestEntityScene::addChildToEntity(Entity2D* parent, Entity2D::Ptr child) {
    if (!parent || !child) return;
    parent->addChild(std::move(child));
}

void TestEntityScene::addTestEntity() {
    auto colors = { sf::Color::Cyan, sf::Color::White, sf::Color(255, 165, 0) };
    static int colorIndex = 0;

    auto newEntity = std::make_unique<TestEntity>("NewEntity", *(colors.begin() + colorIndex));
    colorIndex = (colorIndex + 1) % colors.size();
    newEntity->setPosition(200 + (rand() % 400), 200 + (rand() % 300));
    setupCallbacksRecursive(newEntity.get());
    m_rootEntities.push_back(std::move(newEntity));
}

void TestEntityScene::renderHierarchyUI() {
    ImGui::Begin("Entity Hierarchy Test");

    if (ImGui::Button("Add Test Entity")) {
        addTestEntity();
    }
    ImGui::SameLine();
    if (ImGui::Button("Duplicate Selected") && m_selectedEntity) {
        duplicateSelected();
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete Selected") && m_selectedEntity) {
        deleteSelected();
    }

    ImGui::Separator();

    for (auto& entity : m_rootEntities) {
        renderHierarchyNode(entity.get());
    }

    ImGui::End();
}

void TestEntityScene::renderInspectorUI() {
    ImGui::Begin("Inspector Test");

    if (m_selectedEntity) {
        ImGui::Text("Type: %s", m_selectedEntity->getTypeName());
        ImGui::Text("ID: %d", m_selectedEntity->getUniqueID());
        ImGui::Text("Children: %zu", m_selectedEntity->getChildren().size());
        ImGui::Separator();
        m_selectedEntity->renderEditorUI();

        ImGui::Separator();
        ImGui::Text("Test Operations:");
        if (ImGui::Button("Reset Transform")) {
            m_selectedEntity->setPosition({ 0, 0 });
            m_selectedEntity->setRotation(sf::Angle::Zero);
            m_selectedEntity->setScale({ 1, 1 });
        }
        ImGui::SameLine();
        if (ImGui::Button("Random Pos")) {
            m_selectedEntity->setPosition({
                static_cast<float>(rand() % 800),
                static_cast<float>(rand() % 600)
                });
        }
    }
    else {
        ImGui::Text("No entity selected");
    }

    ImGui::End();
}

void TestEntityScene::updateRecursive(Entity2D* entity, float dt) {
    if (entity->isActive()) {
        entity->update(dt);
    }
    for (const auto& child : entity->getChildren()) {
        updateRecursive(child.get(), dt);
    }
}

void TestEntityScene::renderRecursive(Entity2D* entity, sf::RenderWindow& window) {
    if (entity->isVisible()) {
        entity->render(window);
    }
    for (const auto& child : entity->getChildren()) {
        renderRecursive(child.get(), window);
    }
}

Entity2D* TestEntityScene::findEntityAtPosition(Entity2D* entity, const sf::Vector2f& worldPos) {
    for (auto& child : entity->getChildren()) {
        if (Entity2D* hit = findEntityAtPosition(child.get(), worldPos)) {
            return hit;
        }
    }

    if (entity->containsPoint(worldPos)) {
        return entity;
    }

    return nullptr;
}

void TestEntityScene::renderHierarchyNode(Entity2D* entity) {
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (entity->getChildren().empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }
    if (entity->isSelected()) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    bool open = ImGui::TreeNodeEx(entity->getName().c_str(), flags);

    if (ImGui::IsItemClicked()) {
        selectEntity(entity);
    }

    if (open) {
        for (const auto& child : entity->getChildren()) {
            renderHierarchyNode(child.get());
        }
        ImGui::TreePop();
    }
}

void TestEntityScene::setupCallbacksRecursive(Entity2D* entity) {

    for (const auto& child : entity->getChildren()) {
        setupCallbacksRecursive(child.get());
    }
}
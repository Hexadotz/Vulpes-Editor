#include "sceneManager.hpp"
#include <iostream>

SceneManager::SceneManager() {
    CONSOLE_INFO("[SceneManager] Initialized");

    m_activeScene = createScene("Scene");
}

SceneManager::~SceneManager() {
    CONSOLE_INFO("[SceneManager] Shutting down");
    m_scenes.clear();
}

Scene* SceneManager::createScene(const std::string& name) {
    auto scene = std::make_unique<Scene>(name);
    Scene* ptr = scene.get();
    m_scenes.push_back(std::move(scene));
    

    Vulpes::EventBus::instance().post<Vulpes::SceneCreatedEvent>(name);

    CONSOLE_INFO("[SceneManager] Created scene: " + name);
    return ptr;
}

void SceneManager::removeScene(Scene* scene) {
    auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
        [scene](const Scene::Ptr& ptr) { return ptr.get() == scene; });

    if (it != m_scenes.end()) {
        if (m_activeScene == scene) {
            m_activeScene = nullptr;
        }
        m_scenes.erase(it);
        CONSOLE_INFO("[SceneManager] Removed scene");
    }
}

void SceneManager::setActiveScene(Scene* scene) {
    m_activeScene = scene;

    if (m_activeScene) {
        Vulpes::EventBus::instance().post<Vulpes::SceneActivatedEvent>(
            m_activeScene->getName()
        );
    }
}

Scene* SceneManager::findScene(const std::string& name) const {
    for (const auto& scene : m_scenes) {
        if (scene->getName() == name) {
            return scene.get();
        }
    }
    return nullptr;
}

void SceneManager::update(float deltaTime) {
    if (m_activeScene) {
        m_activeScene->update(deltaTime);
    }
}

void SceneManager::render(sf::RenderWindow& window) {
    if (m_activeScene) {
        m_activeScene->render(window);
    }
}
#pragma once
#include "Scene.hpp"
#include <vector>
#include "../../UI/uiManager.h"

class SceneManager {
private:
    std::vector<Scene::Ptr> m_scenes;
    Scene* m_activeScene = nullptr;

public:
    SceneManager();
    ~SceneManager();

    // Scene management
    Scene* createScene(const std::string& name = "New Scene");
    void removeScene(Scene* scene);
    void setActiveScene(Scene* scene);
    Scene* getActiveScene() const { return m_activeScene; }

    const std::vector<Scene::Ptr>& getScenes() const { return m_scenes; }
    Scene* findScene(const std::string& name) const;

    void update(float deltaTime);
    void render(sf::RenderWindow& window);

    static SceneManager& instance() {
        static SceneManager manager;
        return manager;
    }
};
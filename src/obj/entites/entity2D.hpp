#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>
#include <functional>
#include <imgui.h>
#include <algorithm>
#include <cassert>
#include <iostream>

class Scene;

class Entity2D {
public:
    using Ptr = std::unique_ptr<Entity2D>;
    using PropertyChangedCallback = std::function<void(Entity2D*, const std::string& propertyName)>;
    void clearCallbacks();

protected:
    std::string m_name;
    sf::Transformable m_transform;
    Entity2D* m_parent = nullptr;
    std::vector<Ptr> m_children;

    bool m_isActive = true;
    bool m_isSelected = false;
    bool m_isVisible = true;


    int m_zOrder = 0;
    int m_uniqueID;

    bool m_showTransform = true;
    bool m_lockSelection = false;

    PropertyChangedCallback m_onPropertyChanged;

    static int s_nextID;

public:
    Entity2D(const std::string& name = "Entity");
    virtual ~Entity2D();

    Entity2D(const Entity2D&) = delete;
    Entity2D& operator=(const Entity2D&) = delete;

    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void renderEditorUI() = 0;
    virtual Ptr clone() const = 0;
    virtual const char* getTypeName() const = 0;

    virtual void onSelected() {}
    virtual void onDeselected() {}
    virtual void onTransformChanged() {}
    virtual sf::FloatRect getLocalBounds() const { return sf::FloatRect(); }
    virtual sf::FloatRect getGlobalBounds() const;
    virtual bool containsPoint(const sf::Vector2f& worldPoint) const;

    void setPosition(const sf::Vector2f& position);
    void setPosition(float x, float y);
    void setRotation(sf::Angle angle);
    void setScale(const sf::Vector2f& scale);
    void setScale(float x, float y);
    void setOrigin(const sf::Vector2f& origin);
    void move(const sf::Vector2f& offset);
    void rotate(sf::Angle angle);
    void scale(const sf::Vector2f& factor);

    sf::Vector2f getPosition() const;
    sf::Angle getRotation() const;
    sf::Vector2f getScale() const;
    sf::Vector2f getOrigin() const;
    sf::Vector2f getWorldPosition() const;

    sf::Vector2f worldToLocal(const sf::Vector2f& worldPoint) const;
    sf::Vector2f localToWorld(const sf::Vector2f& localPoint) const;

    void setParent(Entity2D* parent);
    Entity2D* getParent() const { return m_parent; }
    void addChild(Ptr child);
    Ptr removeChild(Entity2D* child);
    const std::vector<Ptr>& getChildren() const { return m_children; }
    Entity2D* findChild(const std::string& name) const;
    Entity2D* findChildByID(int id) const;
    bool isDescendantOf(Entity2D* entity) const;

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name);

    int getUniqueID() const { return m_uniqueID; }

    bool isActive() const { return m_isActive; }
    void setActive(bool active);

    bool isSelected() const { return m_isSelected; }
    void setSelected(bool selected);

    bool isVisible() const { return m_isVisible; }
    void setVisible(bool visible);

    int getZOrder() const { return m_zOrder; }
    void setZOrder(int z);

    bool getLockSelection() const { return m_lockSelection; }
    void setLockSelection(bool lock) { m_lockSelection = lock; }

    void setPropertyChangedCallback(PropertyChangedCallback callback) {
        m_onPropertyChanged = callback;
    }

    virtual void renderTransformUI();

    void markTransformDirty();

    const sf::Transform& getWorldTransform() const {
        if (m_transformDirty) {
            if (m_parent) {
                m_cachedWorldTransform = m_parent->getWorldTransform() * m_transform.getTransform();
            }
            else {
                m_cachedWorldTransform = m_transform.getTransform();
            }
            m_transformDirty = false;
        }
        return m_cachedWorldTransform;
    }

protected:
    mutable sf::Transform m_cachedWorldTransform;
    mutable bool m_transformDirty = true; 

    void notifyPropertyChanged(const std::string& propertyName);
    virtual void onTransformChangedInternal();
};

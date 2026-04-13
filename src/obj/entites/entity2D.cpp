#include "entity2D.hpp"

int Entity2D::s_nextID = 0;


Entity2D::Entity2D(const std::string& name)
    : m_name(name)
    , m_uniqueID(s_nextID++)
    , m_transform()
{
    // nnnnn
}


Entity2D::~Entity2D() {
    PropertyChangedCallback empty;
    m_onPropertyChanged.swap(empty);

    // Detach from parent
    if (m_parent) {
        m_parent->removeChild(this);
    }
}

void Entity2D::clearCallbacks() {
    m_onPropertyChanged = nullptr;

    for (auto& child : m_children) {
        if (child) {
            child->clearCallbacks();
        }
    }
}

void Entity2D::notifyPropertyChanged(const std::string& propertyName) {
    if (m_onPropertyChanged) {
        try {
            m_onPropertyChanged(this, propertyName);
        }
        catch (...) {
            // Catch any exceptions from callback
            std::cout << "[Entity2D] Exception in property callback" << std::endl;
            m_onPropertyChanged = nullptr;
        }
    }
}

void Entity2D::setPosition(const sf::Vector2f& position) {
    if (m_transform.getPosition() != position) {
        m_transform.setPosition(position);
        markTransformDirty();
        notifyPropertyChanged("Position");
        onTransformChangedInternal();
    }
}

void Entity2D::setPosition(float x, float y) {
    setPosition(sf::Vector2f(x, y));
}

void Entity2D::setRotation(sf::Angle angle) {
    if (m_transform.getRotation() != angle) {
        m_transform.setRotation(angle);
        markTransformDirty();
        notifyPropertyChanged("Rotation");
        onTransformChangedInternal();
    }
}

void Entity2D::setScale(const sf::Vector2f& scale) {
    if (m_transform.getScale() != scale) {
        m_transform.setScale(scale);
        markTransformDirty();
        notifyPropertyChanged("Scale");
        onTransformChangedInternal();
    }
}

void Entity2D::setScale(float x, float y) {
    setScale(sf::Vector2f(x, y));
}

void Entity2D::setOrigin(const sf::Vector2f& origin) {
    if (m_transform.getOrigin() != origin) {
        m_transform.setOrigin(origin);
        markTransformDirty();
        notifyPropertyChanged("Origin");
        onTransformChangedInternal();
    }
}

void Entity2D::move(const sf::Vector2f& offset) {
    if (offset != sf::Vector2f(0, 0)) {
        m_transform.move(offset);
        markTransformDirty();
        notifyPropertyChanged("Position");
        onTransformChangedInternal();
    }
}

void Entity2D::rotate(sf::Angle angle) {
    if (angle != sf::Angle::Zero) {
        m_transform.rotate(angle);
        markTransformDirty();
        notifyPropertyChanged("Rotation");
        onTransformChangedInternal();
    }
}

void Entity2D::scale(const sf::Vector2f& factor) {
    if (factor != sf::Vector2f(1, 1)) {
        m_transform.scale(factor);
        markTransformDirty();
        notifyPropertyChanged("Scale");
        onTransformChangedInternal();
    }
}

// Transform getters
sf::Vector2f Entity2D::getPosition() const {
    return m_transform.getPosition();
}

sf::Angle Entity2D::getRotation() const {
    return m_transform.getRotation();
}

sf::Vector2f Entity2D::getScale() const {
    return m_transform.getScale();
}

sf::Vector2f Entity2D::getOrigin() const {
    return m_transform.getOrigin();
}

sf::Vector2f Entity2D::getWorldPosition() const {
    return getWorldTransform().transformPoint(sf::Vector2f(0, 0));
}

sf::FloatRect Entity2D::getGlobalBounds() const {
    sf::FloatRect localBounds = getLocalBounds();
    if (localBounds == sf::FloatRect()) {
        return sf::FloatRect();
    }

    return getWorldTransform().transformRect(localBounds);
}

bool Entity2D::containsPoint(const sf::Vector2f& worldPoint) const {
    sf::FloatRect bounds = getGlobalBounds();
    return bounds.contains(worldPoint);
}

// Coordinate conversion
sf::Vector2f Entity2D::worldToLocal(const sf::Vector2f& worldPoint) const {
    return getWorldTransform().getInverse().transformPoint(worldPoint);
}

sf::Vector2f Entity2D::localToWorld(const sf::Vector2f& localPoint) const {
    return getWorldTransform().transformPoint(localPoint);
}

void Entity2D::setParent(Entity2D* parent) {
    if (m_parent == parent) return;

    // Check for circular reference
    if (parent && parent->isDescendantOf(this)) {
        return;
    }

    if (m_parent) {
        m_parent->removeChild(this);
    }

    m_parent = parent;
    if (m_parent) {
        // This entity will be owned by the new parent
        // The old owner must transfer ownership
    }

    markTransformDirty();
    notifyPropertyChanged("Parent");
}

void Entity2D::addChild(Ptr child) {
    if (!child) return;

    // Check if already a child
    if (child->m_parent == this) return;

    // Remove from old parent
    if (child->m_parent) {
        child->m_parent->removeChild(child.get());
    }

    child->m_parent = this;
    child->markTransformDirty();
    m_children.push_back(std::move(child));
}

Entity2D::Ptr Entity2D::removeChild(Entity2D* child) {
    auto it = std::find_if(m_children.begin(), m_children.end(),
        [child](const Ptr& ptr) { return ptr.get() == child; });

    if (it != m_children.end()) {
        Ptr removed = std::move(*it);
        m_children.erase(it);
        removed->m_parent = nullptr;
        removed->markTransformDirty();
        return removed;
    }

    return nullptr;
}

Entity2D* Entity2D::findChild(const std::string& name) const {
    for (const auto& child : m_children) {
        if (child->m_name == name) {
            return child.get();
        }

        // Recursively search children
        if (Entity2D* found = child->findChild(name)) {
            return found;
        }
    }
    return nullptr;
}

Entity2D* Entity2D::findChildByID(int id) const {
    if (m_uniqueID == id) {
        return const_cast<Entity2D*>(this);
    }

    for (const auto& child : m_children) {
        if (Entity2D* found = child->findChildByID(id)) {
            return found;
        }
    }
    return nullptr;
}

bool Entity2D::isDescendantOf(Entity2D* entity) const {
    if (!entity) return false;

    const Entity2D* current = this;
    while (current->m_parent) {
        if (current->m_parent == entity) {
            return true;
        }
        current = current->m_parent;
    }
    return false;
}

// Property setters with notifications
void Entity2D::setName(const std::string& name) {
    if (m_name != name) {
        m_name = name;
        notifyPropertyChanged("Name");
    }
}

void Entity2D::setActive(bool active) {
    if (m_isActive != active) {
        m_isActive = active;
        notifyPropertyChanged("Active");
    }
}

void Entity2D::setSelected(bool selected) {
    if (m_isSelected != selected) {
        m_isSelected = selected;
        if (selected) {
            onSelected();
        }
        else {
            onDeselected();
        }
        notifyPropertyChanged("Selected");
    }
}

void Entity2D::setVisible(bool visible) {
    if (m_isVisible != visible) {
        m_isVisible = visible;
        notifyPropertyChanged("Visible");
    }
}

void Entity2D::setZOrder(int z) {
    if (m_zOrder != z) {
        m_zOrder = z;
        notifyPropertyChanged("ZOrder");
    }
}

// Transform UI
void Entity2D::renderTransformUI() {
    if (!m_showTransform) return;

    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Position
        sf::Vector2f pos = getPosition();
        float posArr[2] = { pos.x, pos.y };
        if (ImGui::DragFloat2("Position", posArr, 0.1f)) {
            setPosition(posArr[0], posArr[1]);
        }

        // Rotation (in degrees)
        float rotation = getRotation().asDegrees();
        if (ImGui::DragFloat("Rotation", &rotation, 1.0f, 0.0f, 360.0f)) {
            setRotation(sf::degrees(rotation));
        }

        // Scale
        sf::Vector2f scale = getScale();
        float scaleArr[2] = { scale.x, scale.y };
        if (ImGui::DragFloat2("Scale", scaleArr, 0.01f, 0.01f, 10.0f)) {
            setScale(scaleArr[0], scaleArr[1]);
        }

        // Origin
        sf::Vector2f origin = getOrigin();
        float originArr[2] = { origin.x, origin.y };
        if (ImGui::DragFloat2("Origin", originArr, 0.1f)) {
            setOrigin(sf::Vector2f(originArr[0], originArr[1]));
        }

        ImGui::Separator();

        // Z-Order
        int zOrder = getZOrder();
        if (ImGui::DragInt("Z-Order", &zOrder, 1, -100, 100)) {
            setZOrder(zOrder);
        }

        // Visibility
        bool visible = isVisible();
        if (ImGui::Checkbox("Visible", &visible)) {
            setVisible(visible);
        }

        // World position info (read-only)
        ImGui::Text("World Position: (%.2f, %.2f)",
            getWorldPosition().x, getWorldPosition().y);
    }
}

void Entity2D::markTransformDirty() {
    m_transformDirty = true;

    for (auto& child : m_children) {
        child->markTransformDirty();
    }
}

void Entity2D::onTransformChangedInternal() {
    onTransformChanged();

    // Notify children that their world transform is now dirty
    for (auto& child : m_children) {
        child->markTransformDirty();
        child->onTransformChangedInternal();
    }
}
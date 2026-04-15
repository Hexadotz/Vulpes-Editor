#pragma once
#include <string>
#include <memory>

namespace Vulpes {

    enum class EventType {
        EntityCreated,
        EntityDestroyed,
        EntityRenamed,
        EntityReparented,
        EntityDuplicated,

        TransformChanged,
        PositionChanged,
        RotationChanged,
        ScaleChanged,

        SelectionChanged,

        SceneCreated,
        SceneDestroyed,
        SceneActivated,
        SceneModified,

        Unknown
    };

    class Event {
    public:
        EventType type = EventType::Unknown;
        float timestamp = 0.0f;

        Event(EventType t = EventType::Unknown) : type(t) {}
        virtual ~Event() = default;

        // For logging/debugging
        virtual std::string toString() const { return "Event"; }
    };


    struct EntityCreatedEvent : public Event {
        int entityID;
        std::string entityName;
        std::string entityType;

        EntityCreatedEvent() : Event(EventType::EntityCreated) {}

        EntityCreatedEvent(int id, const std::string& name, const std::string& type)
            : Event(EventType::EntityCreated), entityID(id), entityName(name), entityType(type) {
        }

        std::string toString() const override {
            return "EntityCreated: " + entityName + " (ID: " + std::to_string(entityID) + ")";
        }
    };

    struct EntityDestroyedEvent : public Event {
        int entityID;
        std::string entityName;

        EntityDestroyedEvent() : Event(EventType::EntityDestroyed) {}

        EntityDestroyedEvent(int id, const std::string& name)
            : Event(EventType::EntityDestroyed), entityID(id), entityName(name) {
        }

        std::string toString() const override {
            return "EntityDestroyed: " + entityName + " (ID: " + std::to_string(entityID) + ")";
        }
    };

    struct EntityRenamedEvent : public Event {
        int entityID;
        std::string oldName;
        std::string newName;

        EntityRenamedEvent() : Event(EventType::EntityRenamed) {}

        EntityRenamedEvent(int id, const std::string& oldN, const std::string& newN)
            : Event(EventType::EntityRenamed), entityID(id), oldName(oldN), newName(newN) {
        }

        std::string toString() const override {
            return "EntityRenamed: " + oldName + " -> " + newName + " (ID: " + std::to_string(entityID) + ")";
        }
    };

    struct EntityReparentedEvent : public Event {
        int entityID;
        std::string entityName;
        int oldParentID;
        int newParentID;

        EntityReparentedEvent() : Event(EventType::EntityReparented) {}

        EntityReparentedEvent(int id, const std::string& name, int oldP, int newP)
            : Event(EventType::EntityReparented), entityID(id), entityName(name),
            oldParentID(oldP), newParentID(newP) {
        }

        std::string toString() const override {
            return "EntityReparented: " + entityName + " (ID: " + std::to_string(entityID) + ")";
        }
    };

    struct EntityDuplicatedEvent : public Event {
        int originalID;
        int newID;
        std::string entityName;

        EntityDuplicatedEvent() : Event(EventType::EntityDuplicated) {}

        EntityDuplicatedEvent(int origID, int newID, const std::string& name)
            : Event(EventType::EntityDuplicated), originalID(origID), newID(newID), entityName(name) {
        }

        std::string toString() const override {
            return "EntityDuplicated: " + entityName + " (Original: " + std::to_string(originalID) +
                ", New: " + std::to_string(newID) + ")";
        }
    };


    struct TransformChangedEvent : public Event {
        int entityID;
        std::string property; 

        TransformChangedEvent() : Event(EventType::TransformChanged) {}

        TransformChangedEvent(int id, const std::string& prop)
            : Event(EventType::TransformChanged), entityID(id), property(prop) {
        }

        std::string toString() const override {
            return "TransformChanged: ID " + std::to_string(entityID) + " - " + property;
        }
    };


    struct SelectionChangedEvent : public Event {
        int oldEntityID;
        int newEntityID;

        SelectionChangedEvent() : Event(EventType::SelectionChanged), oldEntityID(-1), newEntityID(-1) {}

        SelectionChangedEvent(int oldID, int newID)
            : Event(EventType::SelectionChanged), oldEntityID(oldID), newEntityID(newID) {
        }

        std::string toString() const override {
            return "SelectionChanged: " + std::to_string(oldEntityID) + " -> " + std::to_string(newEntityID);
        }
    };

    struct SceneCreatedEvent : public Event {
        std::string sceneName;

        SceneCreatedEvent() : Event(EventType::SceneCreated) {}
        SceneCreatedEvent(const std::string& name)
            : Event(EventType::SceneCreated), sceneName(name) {
        }

        std::string toString() const override {
            return "SceneCreated: " + sceneName;
        }
    };

    struct SceneActivatedEvent : public Event {
        std::string sceneName;

        SceneActivatedEvent() : Event(EventType::SceneActivated) {}
        SceneActivatedEvent(const std::string& name)
            : Event(EventType::SceneActivated), sceneName(name) {
        }

        std::string toString() const override {
            return "SceneActivated: " + sceneName;
        }
    };

} 
#pragma once
#include "event.hpp"
#include "eventListener.hpp"
#include <vector>
#include <queue>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace Vulpes {

    class EventBus {
    public:
        static EventBus& instance() {
            static EventBus bus;
            return bus;
        }

        template<typename T, typename... Args>
        void post(Args&&... args) {
            auto event = std::make_unique<T>(std::forward<Args>(args)...);
            postEvent(std::move(event));
        }

        void subscribe(EventType type, EventListener* listener) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_subscribers[type].push_back(listener);
        }

        void unsubscribe(EventListener* listener) {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (auto& pair : m_subscribers) {
                auto& vec = pair.second;
                vec.erase(std::remove(vec.begin(), vec.end(), listener), vec.end());
            }
        }

        void process() {
            std::lock_guard<std::mutex> lock(m_mutex);

            while (!m_eventQueue.empty()) {
                auto& event = m_eventQueue.front();

                auto it = m_subscribers.find(event->type);
                if (it != m_subscribers.end()) {
                    for (auto* listener : it->second) {
                        if (listener) {
                            listener->onEvent(*event);
                        }
                    }
                }

                m_eventQueue.pop();
            }
        }

        void clear() {
            std::lock_guard<std::mutex> lock(m_mutex);
            while (!m_eventQueue.empty()) {
                m_eventQueue.pop();
            }
        }

    private:
        EventBus() = default;
        

        void postEvent(std::unique_ptr<Event> event) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_eventQueue.push(std::move(event));
        }

        std::queue<std::unique_ptr<Event>> m_eventQueue;
        std::unordered_map<EventType, std::vector<EventListener*>> m_subscribers;
        std::mutex m_mutex;
    };
}
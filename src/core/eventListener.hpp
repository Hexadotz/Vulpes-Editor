#pragma once
#include "event.hpp"

namespace Vulpes {

    class EventListener {
    public:
        virtual ~EventListener() = default;
        virtual void onEvent(const Event& event) = 0;
    };

} // namespace Vulpes
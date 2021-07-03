#pragma once

namespace event_triggers {

class IEventTrigger {
  public:
    virtual ~IEventTrigger() = default;

    virtual void onEvent() = 0;
};

} // event_triggers

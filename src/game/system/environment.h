#pragma once

#include <unordered_set>
#include <string>
#include <map>
#include <memory>

#include "event_triggers/event_trigger.h"

class Environment {
  public:
    bool getFlag(std::string name);
    void setFlag(std::string name);

    void addConditionalEvent(std::string condition, std::shared_ptr<event_triggers::IEventTrigger> event);
    void triggerConditionalEvents();
    void clearConditionalEvents();

  private:
    std::unordered_set<std::string> flags_;

    // Conditional entities
    std::unordered_set<std::string> triggered_conditions_;
    std::multimap<std::string, std::shared_ptr<event_triggers::IEventTrigger>> conditional_events_;
};

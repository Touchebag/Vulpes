#pragma once

#include "event_trigger.h"

#include "components/actions/actions.h"

namespace event_triggers {

class EnableAction : public IEventTrigger {
  public:
    EnableAction(std::weak_ptr<Actions> actions_instance, Actions::Action action);

    void onEvent() override;

  private:
    std::weak_ptr<Actions> actions_instance_;
    Actions::Action action_;
};

} // event_triggers

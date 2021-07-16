#include "enable_action.h"

namespace event_triggers {

EnableAction::EnableAction(std::weak_ptr<Actions> actions_instance, Actions::Action action) :
    actions_instance_(actions_instance),
    action_(action) {
}

void EnableAction::onEvent() {
    if (auto act_inst = actions_instance_.lock()) {
        act_inst->enableAction(action_, true);
    }
}

} // event_triggers

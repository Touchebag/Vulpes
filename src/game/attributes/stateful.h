#pragma once

#include "state.h"
#include "attributes/animated.h"

class StatefulEntity {
  public:
    StatefulEntity();

    // TODO Make "anim" member
    virtual void incomingEvent(state::Event event, std::weak_ptr<AnimatedEntity> anim) final;

    virtual const state::Properties& getStateProperties() final;

  protected:
    unsigned int frame_counter_;

  private:
    State* current_state_;
    std::unordered_map<std::string, State> state_list_;
};

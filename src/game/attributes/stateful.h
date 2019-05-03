#pragma once

#include "state.h"

class StatefulEntity {
  public:
    StatefulEntity();

    virtual void updateState() final;

    virtual void incomingEvent(state::Event event) final;

    virtual const state::Properties& getStateProperties() final;

    virtual std::string getCurrentSprite() final;

  protected:
    unsigned int frame_counter_;

  private:
    State* current_state_;
    std::unordered_map<std::string, State> state_list_;
};

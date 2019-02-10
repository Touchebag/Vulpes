#pragma once

#include <vector>

#include "state_object.h"

class StateList {
  public:
    static StateList& getInstance();

    std::vector<StateObject>& getObjects();
  private:
    std::vector<StateObject> objects_;
};

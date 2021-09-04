#pragma once

#include <vector>

#include "components/component_store.h"

class Interpreter {
  public:
    struct ExtraInputData {
        std::shared_ptr<ComponentStore> this_components = std::make_shared<ComponentStore>();
        int frame_timer = 0;
    };

    static int executeProgram(std::vector<int> program, ExtraInputData extra_data);
};

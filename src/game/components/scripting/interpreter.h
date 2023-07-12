#pragma once

#include <vector>

#include "components/component_store.h"
#include "program.h"

class Interpreter {
  public:
    struct ExtraInputData {
        std::shared_ptr<ComponentStore> this_components = std::make_shared<ComponentStore>();
        unsigned int frame_timer = 0;
        std::shared_ptr<scripting::VariableMap> variables = std::make_shared<scripting::VariableMap>();
    };

    static int executeProgram(Program program, ExtraInputData extra_data);
};

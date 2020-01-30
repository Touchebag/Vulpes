#pragma once

#include "json.hpp"

#include "value.h"

namespace ai {
namespace condition {

class Dynamic : public Value {
  public:
    Dynamic(nlohmann::json j);

    int getValue() const override;

  private:
    int (*int_function_)();
};

} // condition
} // ai

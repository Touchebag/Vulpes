#pragma once

#include "json.hpp"

#include "value.h"

namespace ai {
namespace condition {

class PlayerValue : public Value {
  public:
    PlayerValue(std::vector<std::string> arguments);

    int getValue(std::weak_ptr<Transform> trans) const override;

  private:
    int (*int_function_)();
};

} // condition
} // ai

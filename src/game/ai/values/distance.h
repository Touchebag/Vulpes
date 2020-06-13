#pragma once

#include "json.hpp"

#include "value.h"

namespace ai {
namespace condition {

class Distance : public Value {
  public:
    Distance(nlohmann::json j);

    int getValue(std::weak_ptr<Transform> trans) const override;

  private:
    int (Distance::*int_function_)(std::weak_ptr<Transform>) const;

    int getDistanceX(std::weak_ptr<Transform> trans) const;

    std::shared_ptr<const Value> value1_;
    std::shared_ptr<const Value> value2_;
};

} // condition
} // ai

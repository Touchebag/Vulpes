#pragma once

#include "json.hpp"

#include "components/transform.h"
#include "value.h"

namespace ai {
namespace condition {

class This : public Value {
  public:
    This(nlohmann::json j);

    int getValue(std::weak_ptr<Transform> trans) const override;

  private:
    int (This::*int_function_)(std::weak_ptr<Transform>) const;
    std::weak_ptr<Transform> trans_;

    int positionX(std::weak_ptr<Transform> this_trans) const;
};

} // condition
} // ai

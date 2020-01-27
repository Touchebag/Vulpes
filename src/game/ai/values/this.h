#pragma once

#include "json.hpp"

#include "components/transform.h"
#include "value.h"

namespace ai {
namespace condition {

class This : public Value {
  public:
    This(nlohmann::json j, std::weak_ptr<Transform> trans);

    operator int() const override;

  private:
    int (This::*int_function_)() const;
    std::weak_ptr<Transform> trans_;

    int positionX() const;
};

} // condition
} // ai

#pragma once

#include "json.hpp"

#include "components/transform.h"

namespace ai {
namespace condition {

class Value {
  public:
    static std::unique_ptr<const Value> createFromJson(nlohmann::json j);

    virtual int getValue(std::weak_ptr<Transform> this_trans) const = 0;
};

} // condition
} // ai

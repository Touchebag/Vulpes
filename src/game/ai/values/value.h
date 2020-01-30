#pragma once

#include "json.hpp"

#include "components/transform.h"

namespace ai {
namespace condition {

class Value {
  public:
    static std::unique_ptr<const Value> loadFromJson(nlohmann::json j, std::weak_ptr<Transform> trans);

    virtual int getValue() const = 0;
};

} // condition
} // ai

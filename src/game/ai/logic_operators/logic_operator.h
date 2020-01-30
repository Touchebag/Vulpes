#pragma once

#include "json.hpp"

#include "components/transform.h"

namespace ai {
namespace condition {

class LogicalOperator {
  public:
    static std::unique_ptr<const LogicalOperator> loadFromJson(nlohmann::json j, std::weak_ptr<Transform> trans);

    virtual bool getValue() const = 0;
};

} // condition
} // ai

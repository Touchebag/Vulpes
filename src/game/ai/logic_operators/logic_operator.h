#pragma once

#include "json.hpp"

#include "components/transform.h"

namespace ai {
namespace condition {

class LogicalOperator {
  public:
    static std::shared_ptr<const LogicalOperator> loadFromJson(nlohmann::json j);

    virtual bool getValue(std::weak_ptr<Transform> trans, unsigned int& frame_timer) const = 0;
};

} // condition
} // ai

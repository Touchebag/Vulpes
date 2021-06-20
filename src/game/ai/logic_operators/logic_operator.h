#pragma once

#include "json.hpp"

#include "components/transform.h"
#include "components/animation.h"

namespace ai {
namespace condition {

class LogicalOperator {
  public:
    struct aiValues {
        std::weak_ptr<Transform> trans = {};
        std::weak_ptr<Collision> coll = {};
        std::weak_ptr<Animation> anim = {};

        unsigned int& frame_timer;
    };

    virtual ~LogicalOperator() = default;

    static std::shared_ptr<const LogicalOperator> createFromJson(nlohmann::json j);

    virtual bool getValue(aiValues& values) const = 0;

  private:
    static std::shared_ptr<const LogicalOperator> createFromString(const std::string& str);
};

} // condition
} // ai

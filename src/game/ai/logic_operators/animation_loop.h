#pragma once

#include "logic_operator.h"
#include "ai/values/value.h"

#include "utils/log.h"

namespace ai {
namespace condition {

class AnimationLoop : public LogicalOperator {
  public:
    AnimationLoop() = default;

    bool getValue(aiValues& values) const override {
        if (auto anim = values.anim.lock()) {
            return anim->hasAnimationLooped();
        } else {
            return false;
        }
    }
};

} // condition
} // ai

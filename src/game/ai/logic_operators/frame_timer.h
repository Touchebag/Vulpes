#pragma once

#include "logic_operator.h"
#include "ai/values/value.h"

namespace ai {
namespace condition {

class FrameTimer : public LogicalOperator {
  public:
    FrameTimer(std::unique_ptr<const Value> frame_timeout) :
        frame_timeout_(std::move(frame_timeout)) {}

    bool getValue(std::weak_ptr<Transform> trans, unsigned int& frame_timer) const override {
        if (frame_timer > static_cast<unsigned int>(frame_timeout_->getValue(trans))) {
            frame_timer = 0;
            return true;
        } else {
            return false;
        }
    }

  private:
    const std::unique_ptr<const Value> frame_timeout_;
};

} // condition
} // ai

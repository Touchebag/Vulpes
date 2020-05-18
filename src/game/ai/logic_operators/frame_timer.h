#pragma once

#include "logic_operator.h"
#include "ai/values/value.h"

#include "utils/log.h"

namespace ai {
namespace condition {

class FrameTimer : public LogicalOperator {
  public:
    FrameTimer(std::unique_ptr<const Value> frame_timeout) :
        frame_timeout_(std::move(frame_timeout)) {}

    bool getValue(aiValues& values) const override {
        if (values.frame_timer > static_cast<unsigned int>(frame_timeout_->getValue(values.trans))) {
            values.frame_timer = 0;
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

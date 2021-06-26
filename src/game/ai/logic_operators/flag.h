#pragma once

#include "logic_operator.h"

#include "system/system.h"

#include "utils/log.h"

namespace ai {
namespace condition {

class Flag : public LogicalOperator {
  public:
    Flag(std::string condition) {
        condition_ = condition;
    }

    bool getValue(aiValues& /* values */) const override {
        return System::getEnvironment()->getFlag(condition_);
    }

  private:
    std::string condition_;
};

} // condition
} // ai

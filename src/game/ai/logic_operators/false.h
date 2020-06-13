#pragma once

#include "logic_operator.h"
#include "ai/values/value.h"

namespace ai {
namespace condition {

class False : public LogicalOperator {
  public:
    bool getValue(aiValues& values) const override {
        (void)values;
        return false;
    }
};

} // condition
} // ai

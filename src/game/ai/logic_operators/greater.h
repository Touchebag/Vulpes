#pragma once

#include "logic_operator.h"
#include "ai/values/value.h"

namespace ai {
namespace condition {

class Greater : public LogicalOperator {
  public:
    Greater(Value& value1, Value& value2) :
        value1_(value1),
        value2_(value2) {}

    operator bool() const override {
        return value1_ > value2_;
    }

  private:
    const Value& value1_;
    const Value& value2_;
};

} // condition
} // ai

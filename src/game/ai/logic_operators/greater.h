#pragma once

#include "logic_operator.h"
#include "ai/values/value.h"

namespace ai {
namespace condition {

class Greater : public LogicalOperator {
  public:
    Greater(std::unique_ptr<const Value> value1, std::unique_ptr<const Value> value2) :
        value1_(std::move(value1)),
        value2_(std::move(value2)) {}

    bool getValue() const override {
        return value1_->getValue() > value2_->getValue();
    }

  private:
    const std::unique_ptr<const Value> value1_;
    const std::unique_ptr<const Value> value2_;
};

} // condition
} // ai

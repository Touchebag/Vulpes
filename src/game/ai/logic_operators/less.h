#pragma once

#include "logic_operator.h"
#include "ai/values/value.h"

namespace ai {
namespace condition {

class Less : public LogicalOperator {
  public:
    Less(std::unique_ptr<const Value> value1, std::unique_ptr<const Value> value2) :
        value1_(std::move(value1)),
        value2_(std::move(value2)) {}

    bool getValue(aiValues& values) const override {
        return value1_->getValue(values.trans) < value2_->getValue(values.trans);
    }

  private:
    const std::unique_ptr<const Value> value1_;
    const std::unique_ptr<const Value> value2_;
};

} // condition
} // ai

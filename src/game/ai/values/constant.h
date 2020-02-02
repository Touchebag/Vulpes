#pragma once

#include "value.h"

namespace ai {
namespace condition {

class Constant : public Value {
  public:
    Constant(int value) : value_(value) {}

    int getValue(std::weak_ptr<Transform>) const override {
        return value_;
    }

  private:
    const int value_;
};

} // condition
} // ai

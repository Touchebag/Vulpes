#pragma once

#include "json.hpp"

#include "components/transform.h"

namespace ai {
namespace condition {

class Value {
  public:
    virtual ~Value() = default;

    static std::unique_ptr<const Value> createFromString(std::string str);

    virtual int getValue(std::weak_ptr<Transform> this_trans) const = 0;
};

} // condition
} // ai

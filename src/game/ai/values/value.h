#pragma once

namespace ai {
namespace condition {

class Value {
  public:
    virtual operator int() const = 0;
};

} // condition
} // ai

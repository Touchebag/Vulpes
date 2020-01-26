#pragma once

namespace ai {
namespace condition {

class LogicalOperator {
  public:
    virtual operator bool() const = 0;
};

} // condition
} // ai

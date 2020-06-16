#include "distance.h"

#include "system/world.h"

namespace ai {
namespace condition {

Distance::Distance(std::unique_ptr<const Value> value1, std::unique_ptr<const Value> value2) :
    value1_(std::move(value1)),
    value2_(std::move(value2)) {
    int_function_ = &Distance::getDistanceX;
}

int Distance::getValue(std::weak_ptr<Transform> trans) const {
    return (this->*int_function_)(trans);
}

int Distance::getDistanceX(std::weak_ptr<Transform> trans) const {
    return abs(value1_->getValue(trans) - value2_->getValue(trans));
}

} // condition
} // ai

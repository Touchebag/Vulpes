#include "distance.h"

#include "system/world.h"

namespace ai {
namespace condition {

Distance::Distance(nlohmann::json j) {
    if (!j.contains("value1") || !j.contains("value2")) {
        throw std::invalid_argument("Distance: missing values");
    }

    value1_ = Value::createFromJson(j["value1"]);
    value2_ = Value::createFromJson(j["value2"]);

    int_function_ = getDistanceX;
}

int Distance::getValue(std::weak_ptr<Transform> trans) const {
    return (this->*int_function_)(trans);
}

int Distance::getDistanceX(std::weak_ptr<Transform> trans) const {
    return abs(value1_->getValue(trans) - value2_->getValue(trans));
}

} // condition
} // ai

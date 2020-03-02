#include "this.h"

#include "system/world.h"

namespace ai {
namespace condition {

This::This(nlohmann::json /* j */) {
    int_function_ = &This::positionX;
}

int This::getValue(std::weak_ptr<Transform> trans) const {
    return (this->*int_function_)(trans);
}

int This::positionX(std::weak_ptr<Transform> trans) const {
    if (auto ptr = trans.lock()) {
        return ptr->getX();
    } else {
        throw std::runtime_error("Error reading value. Transform ptr invalid.");
    }
}

} // condition
} // ai

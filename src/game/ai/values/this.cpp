#include "this.h"

#include "system/world.h"

namespace ai {
namespace condition {

This::This(nlohmann::json j, std::weak_ptr<Transform> trans) :
    trans_(trans) {
    int_function_ = &This::positionX;
}

int This::getValue() const {
    return (this->*int_function_)();
}

int This::positionX() const {
    if (auto ptr = trans_.lock()) {
        return ptr->getX();
    } else {
        throw std::runtime_error("Error reading value. Transform ptr invalid.");
    }
}

} // condition
} // ai

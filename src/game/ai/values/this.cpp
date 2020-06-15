#include "this.h"

#include "system/world.h"

namespace ai {
namespace condition {

This::This(std::vector<std::string>) {
    // TODO Parse arguments
    int_function_ = &This::positionX;
}

int This::getValue(std::weak_ptr<Transform> this_trans) const {
    return (this->*int_function_)(this_trans);
}

int This::positionX(std::weak_ptr<Transform> this_trans) const {
    if (auto ptr = this_trans.lock()) {
        return ptr->getX();
    } else {
        throw std::runtime_error("Error reading value. Transform ptr invalid.");
    }
}

} // condition
} // ai

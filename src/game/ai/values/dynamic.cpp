#include "dynamic.h"

#include "system/world.h"

namespace ai {
namespace condition {

Dynamic::Dynamic(nlohmann::json j) {
    int_function_ = [] () { return World::IWorldRead::getPlayerPosition().x; };
}

int Dynamic::getValue() const {
    return int_function_();
}

} // condition
} // ai

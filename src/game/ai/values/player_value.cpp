#include "player_value.h"

#include "system/world.h"

namespace ai {
namespace condition {

PlayerValue::PlayerValue(std::vector<std::string>) {
    // TODO Parse input
    int_function_ = [] () { return World::IWorldRead::getPlayerPosition().x; };
}

int PlayerValue::getValue(std::weak_ptr<Transform>) const {
    return int_function_();
}

} // condition
} // ai

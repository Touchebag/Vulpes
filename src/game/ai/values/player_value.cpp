#include "player_value.h"

#include "system/system.h"

namespace ai {
namespace condition {

PlayerValue::PlayerValue(std::vector<std::string>) {
    // TODO Parse input
    int_function_ = [] () { return System::IWorldRead::getPlayerPosition().x; };
}

int PlayerValue::getValue(std::weak_ptr<Transform>) const {
    return int_function_();
}

} // condition
} // ai

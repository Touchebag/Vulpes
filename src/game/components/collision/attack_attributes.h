#pragma once

#include "json.hpp"

namespace collision {

struct AttackAttributes {
    int damage = 0;
    int knockback_x = 0;
    int knockback_y = 0;
};

AttackAttributes parseAttackAttributes(nlohmann::json j);

nlohmann::json dumpAttackAttributes(AttackAttributes attack);

} // collision

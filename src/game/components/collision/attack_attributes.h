#pragma once

#include "nlohmann/json.hpp"

namespace collision {

struct AttackAttributes {
    int damage = 0;
    int knockback_x = 0;
    int knockback_y = 0;
    int invincibility_frames = 0;
};

AttackAttributes parseAttackAttributes(const nlohmann::json& j);

nlohmann::json dumpAttackAttributes(const AttackAttributes& attack);

} // collision

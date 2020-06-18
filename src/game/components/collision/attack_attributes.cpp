#include "attack_attributes.h"

namespace collision {

AttackAttributes parseAttackAttributes(nlohmann::json j) {
    AttackAttributes attack;

    if (j.contains("damage")) {
        attack.damage = j["damage"];
    }

    if (j.contains("knockback_x")) {
        attack.knockback_x = j["knockback_x"];
    }
    if (j.contains("knockback_y")) {
        attack.knockback_y = j["knockback_y"];
    }
    if (j.contains("invincibility")) {
        attack.invincibility_frames = j["invincibility"];
    }

    return attack;
}

nlohmann::json dumpAttackAttributes(AttackAttributes attack) {
    nlohmann::json j;

    if (attack.damage > 0) {
        j["damage"] = attack.damage;
    }

    if (attack.knockback_x != 0) {
        j["knockback_x"] = attack.knockback_x;
    }
    if (attack.knockback_y != 0) {
        j["knockback_y"] = attack.knockback_y;
    }

    if (attack.invincibility_frames > 0) {
        j["invincibility"] = attack.invincibility_frames;
    }

    return j;
}

} // collision

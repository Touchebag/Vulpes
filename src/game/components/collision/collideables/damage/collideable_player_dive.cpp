#include "collideable_player_dive.h"

#include "components/collision/attack_attributes.h"
#include "system/world.h"

#include "utils/log.h"

bool already_added = false;

CollideablePlayerDive::CollideablePlayerDive(std::weak_ptr<ComponentStore> components) :
    CollideableDamage(components) {
}

void CollideablePlayerDive::update() {
    for (auto it : World::IWorldRead::getCollideables(Collideable::CollisionType::ENEMY_HITBOX)) {
        if (collides(it)) {
            if (auto actions = components_.lock()->getComponent<Actions>()) {
                actions->addAction(Actions::Action::AIR_DIVE_BOUNCE);
            }
        }
    }
}

void CollideablePlayerDive::reloadFromJson(nlohmann::json j) {
    if (!j.contains("type") || !(j["type"].get<std::string>() == "player_dive")) {
        throw std::invalid_argument("Player dive invalid type" + j["type"].get<std::string>());
    }

    Collideable::reloadFromJson(j);

    if (!j.contains("attack")) {
        return;
    }

    attack_attributes_ = collision::parseAttackAttributes(j["attack"]);
}

std::optional<nlohmann::json> CollideablePlayerDive::outputToJson() {
    nlohmann::json j = Collideable::outputToJson().value();

    j["attack"] = collision::dumpAttackAttributes(attack_attributes_);

    return j;
}

Collideable::CollisionType CollideablePlayerDive::getType() const {
    return Collideable::CollisionType::PLAYER_DIVE;
}

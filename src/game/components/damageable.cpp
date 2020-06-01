#include "damageable.h"

#include "utils/log.h"
#include "system/world.h"

namespace {

typedef Collision::CollisionType Ctype;

std::map<Ctype, Ctype> type_mapping = {
    {Ctype::PLAYER_HURTBOX, Ctype::ENEMY_HITBOX},
    {Ctype::ENEMY_HITBOX, Ctype::PLAYER_HITBOX}
};

bool knockbackRight(std::shared_ptr<Collision> this_coll, std::shared_ptr<const Collision> other_coll) {
    if (auto this_trans = this_coll->getTransform().lock()) {
        if (auto other_trans = other_coll->getTransform().lock()) {
            // If this is left of hitbox, move left
            return (this_trans->getX() >= other_trans->getX());
        }
    }

    return true;
}

} // namespace

Damageable::Damageable(std::weak_ptr<Collision> hurtbox,
                       std::weak_ptr<Death> death,
                       std::weak_ptr<StatefulEntity> state,
                       std::weak_ptr<MovableEntity> move) :
    hurtbox_(hurtbox),
    death_(death),
    state_(state),
    move_(move) {
}

void Damageable::reloadFromJson(nlohmann::json j) {
    if (j.contains("health")) {
        health_ = j["health"].get<int>();
        initial_health_ = health_;
    }
}

std::optional<nlohmann::json> Damageable::outputToJson() {
    nlohmann::json j;
    j["health"] = initial_health_;

    return j;
}

void Damageable::update() {
    if (auto coll = hurtbox_.lock()) {
        auto hurting_type = type_mapping.at(coll->getType());

        for (auto& it : World::IWorldRead::getCollisions(hurting_type)) {
            if (auto other_coll = it.lock()) {
                if (coll->collides(other_coll)) {
                    auto attributes = other_coll->getAttributes();
                    health_ -= attributes.damage;
                    if (auto state = state_.lock()) {
                        state->incomingEvent(state_utils::Event::DAMAGED);
                        if (auto move = move_.lock()) {
                            bool should_move_right = knockbackRight(coll, other_coll);
                            auto max_movement = move->getMaximumMovement(
                                    attributes.knockback_x * (should_move_right ? 1.0 : -1.0),
                                    attributes.knockback_y);
                            move->move(max_movement.first, max_movement.second);
                            // Face toward damage source, away from knockback direction
                            move->facing_right_ = !should_move_right;
                        }
                    }
                }
            }
        }
    }

    if (isDead()) {
        if (auto death = death_.lock()) {
            death->setDead();
        }
    }
}

int Damageable::getHealth() {
    return health_;
}

bool Damageable::isDead() {
    return health_ <= 0;
}

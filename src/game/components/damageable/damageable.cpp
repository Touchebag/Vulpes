#include "damageable.h"

#include "utils/log.h"
#include "system/world.h"
#include "damageable_player.h"
#include "components/collision/collideables/damage/collideable_damage.h"

namespace {

typedef Collideable::CollisionType Ctype;

std::map<Ctype, Ctype> type_mapping = {
    {Ctype::PLAYER_HURTBOX, Ctype::ENEMY_HITBOX},
    {Ctype::ENEMY_HITBOX, Ctype::PLAYER_HITBOX}
};

bool knockbackRight(std::shared_ptr<Collideable> this_coll, std::shared_ptr<const Collideable> other_coll) {
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
                       std::weak_ptr<RenderableEntity> render,
                       std::weak_ptr<MovableEntity> move) :
    hurtbox_(hurtbox),
    death_(death),
    state_(state),
    render_(render),
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

std::shared_ptr<Damageable> Damageable::createFromJson(nlohmann::json j,
                                                       std::weak_ptr<Collision> coll,
                                                       std::weak_ptr<Death> death,
                                                       std::weak_ptr<StatefulEntity> state,
                                                       std::weak_ptr<RenderableEntity> render,
                                                       std::weak_ptr<MovableEntity> move) {
    if (!j.contains("type")) {
        auto dmg = std::make_shared<Damageable>(coll, death, state, render, move);
        dmg->reloadFromJson(j);
        return dmg;
    }

    if (j["type"].get<std::string>() == "player") {
        auto dmg = std::make_shared<DamageablePlayer>(coll, death, state, render, move);
        dmg->reloadFromJson(j);
        return dmg;
    }

    throw std::invalid_argument(std::string("Invalid type: ") + j["type"].get<std::string>());
}

void Damageable::update() {
    if (invincibility_frame_counter_ > 0) {
        // If this is the last frame remove overlay
        if (--invincibility_frame_counter_ == 0) {
            if (auto render = render_.lock()) {
                render->clearColor();
            }
        }
        return;
    }

    if (auto coll = hurtbox_.lock()->getCollideable()) {
        auto hurting_type = type_mapping.at(coll->getType());

        for (auto& it : World::IWorldRead::getCollideables(hurting_type)) {
            if (auto other_coll = it.lock()) {

                if (coll->collides(other_coll)) {

                    collision::AttackAttributes attributes;
                    if (auto other_coll_damage = std::dynamic_pointer_cast<const CollideableDamage>(other_coll)) {
                        attributes = other_coll_damage->getAttributes();
                    } else {
                        LOGW("Could not cast collision type to get attributes");
                        return;
                    }

                    health_ -= attributes.damage;
                    invincibility_frame_counter_ = attributes.invincibility_frames;

                    if (auto state = state_.lock()) {
                        state->incomingEvent(state_utils::Event::DAMAGED);

                        if (invincibility_frame_counter_ > 0) {
                            if (auto render = render_.lock()) {
                                render->setColor({255, 255, 255, 128});
                            }
                        }

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

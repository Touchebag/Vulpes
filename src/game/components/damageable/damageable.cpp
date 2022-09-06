#include "damageable.h"

#include "utils/log.h"
#include "system/system.h"
#include "damageable_player.h"

#include "components/collision/collideables/collideable_hitbox.h"
#include "components/collision/collideables/collideable_hurtbox.h"

#include "components/component_store.h"

namespace {

bool knockbackRight(std::shared_ptr<Collideable> this_coll, std::shared_ptr<const Collideable> other_coll) {
    if (auto this_trans = this_coll->getTransform().lock()) {
        if (auto other_trans = other_coll->getTransform().lock()) {
            // If this is left of hitbox, move left
            return (this_trans->getX() >= other_trans->getX());
        }
    }

    return true;
}

bool sharesElements(const std::set<int>& first, const std::set<int>& second) {
    for (auto team : first) {
        // If they share a team do not take damage
        if (second.count(team) > 0) {
            return true;
        }
    }

    return false;
}

} // namespace

Damageable::Damageable(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void Damageable::reloadFromJson(nlohmann::json j, File /* file_instance */) {
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

std::shared_ptr<Damageable> Damageable::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File /* file_instance */) {
    if (!j.contains("type")) {
        auto dmg = std::make_shared<Damageable>(components);
        dmg->reloadFromJson(j);
        return dmg;
    }

    if (j["type"].get<std::string>() == "player") {
        auto dmg = std::make_shared<DamageablePlayer>(components);
        dmg->reloadFromJson(j);
        return dmg;
    }

    throw std::invalid_argument(std::string("Invalid type: ") + j["type"].get<std::string>());
}

void Damageable::update() {
    if (invincibility_frame_counter_ > 0) {
        // If this is the last frame remove overlay
        if (--invincibility_frame_counter_ == 0) {
            if (auto render = getComponent<Rendering>()) {
                render->clearColor();
            }
        }
        return;
    }
    if (auto collision = getComponent<Collision>()) {
        if (auto coll = collision->getCollideable()) {
            std::set<int> this_teams;

            if (auto coll_damage = std::dynamic_pointer_cast<const CollideableHurtbox>(coll)) {
                this_teams = coll_damage->getTeams();
            } else {
                LOGW("Could not cast collision type to get teams");
                return;
            }

            for (auto& it : System::IWorldRead::getCollideables(Collideable::CollisionType::HITBOX)) {
                if (auto other_coll = it.lock()) {
                    collision::AttackAttributes other_attr;
                    std::set<int> other_teams;

                    if (auto other_coll_damage = std::dynamic_pointer_cast<const CollideableHitbox>(other_coll)) {
                        other_attr = other_coll_damage->getAttributes();
                        other_teams = other_coll_damage->getTeams();
                    } else {
                        LOGW("Could not cast collision type to get attributes");
                        return;
                    }

                    // If they are on the same team do not take damage
                    if (sharesElements(this_teams, other_teams)) {
                        continue;
                    }

                    if (coll->collides(other_coll)) {
                        health_ -= other_attr.damage;
                        invincibility_frame_counter_ = other_attr.invincibility_frames;

                        if (auto state = getComponent<Stateful>()) {
                            state->incomingEvent(state_utils::Event::DAMAGED);

                            if (invincibility_frame_counter_ > 0) {
                                if (auto render = getComponent<Rendering>()) {
                                    render->setColor({255, 255, 255, 128});
                                }
                            }

                            if (auto move = getComponent<Movement>()) {
                                bool should_move_right = knockbackRight(coll, other_coll);
                                move->move(other_attr.knockback_x * (should_move_right ? 1.0 : -1.0),
                                           other_attr.knockback_y);
                                // Face toward damage source, away from knockback direction
                                move->setFacingRight(!should_move_right);
                            }
                        }
                    }
                }
            }
        }
    }

    if (isDead()) {
        if (auto death = getComponent<Death>()) {
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

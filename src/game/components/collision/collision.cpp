#include "collision.h"

#include "utils/log.h"

#include "collideables/collideable_player_hurtbox.h"
#include "collideables/collideable_transition.h"
#include "collideables/collideable_health.h"
#include "collideables/collideable_collectible.h"
#include "collideables/collideable_interactable.h"

#include "collideables/movement/collideable_static.h"
#include "collideables/movement/collideable_semisolid.h"
#include "collideables/movement/collideable_slope.h"

#include "collideables/damage/collideable_player_hitbox.h"
#include "collideables/damage/collideable_enemy_hitbox.h"

#include "system/world.h"

namespace {

const std::map<std::string, Collideable::CollisionType> string_type_map {
    {"static", Collideable::CollisionType::STATIC},
    {"semi_solid", Collideable::CollisionType::SEMI_SOLID},
    {"slope", Collideable::CollisionType::SLOPE},
    {"player_hurtbox", Collideable::CollisionType::PLAYER_HURTBOX},
    {"player_hitbox", Collideable::CollisionType::PLAYER_HITBOX},
    {"player_dive", Collideable::CollisionType::PLAYER_HITBOX},
    {"enemy_hitbox", Collideable::CollisionType::ENEMY_HITBOX},
    {"transition", Collideable::CollisionType::TRANSITION},
    {"health", Collideable::CollisionType::HEALTH},
    {"collectible", Collideable::CollisionType::COLLECTIBLE},
    {"interactable", Collideable::CollisionType::INTERACTABLE},
};

} // namespace

Collision::Collision(std::weak_ptr<Transform> trans, std::weak_ptr<Actions> actions) :
    trans_(trans),
    actions_(actions) {

    collideable_ = std::make_shared<CollideableStatic>(trans_);
}

void Collision::update() {
    if (temp_coll_) {
        temp_coll_->update();
    }

    collideable_->update();
}

std::shared_ptr<Collision> Collision::createFromJson(nlohmann::json j, std::weak_ptr<Transform> trans, std::weak_ptr<Actions> actions) {
    auto collision = std::make_shared<Collision>(trans, actions);

    collision->setCollideable(j);

    return collision;
}

void Collision::reloadFromJson(nlohmann::json j) {
    collideable_->reloadFromJson(j);
}

std::optional<nlohmann::json> Collision::outputToJson() {
    return collideable_->outputToJson();
}

bool Collision::collides(std::weak_ptr<const Collision> other_entity) {
    if (auto other_coll = other_entity.lock()) {
        return collides(other_coll->getCollideable());
    }

    return false;
}

bool Collision::collides(std::weak_ptr<const Collideable> other_entity) {
    return collideable_->collides(other_entity);
}

Collideable::CollisionType Collision::getType() const {
    return collideable_->getType();
}

std::weak_ptr<const Transform> Collision::getTransform() const {
    return collideable_->getTransform();
}

std::shared_ptr<Collideable> Collision::getCollideable() const {
    return collideable_;
}

void Collision::addTemporaryCollideable(nlohmann::json j) {
    temp_coll_ = Collideable::createFromJson(j, trans_, actions_);

    World::IWorldModify::addCollideable(temp_coll_);
}

void Collision::clearTemporaryCollideables() {
    temp_coll_ = nullptr;
}

void Collision::setCollideable(nlohmann::json j) {
    auto coll = Collideable::createFromJson(j, trans_, actions_);

    if (coll) {
        collideable_ = coll;
    } else {
        // This should never happen as createFromJson will throw
        // Message here for debug reasons
        LOGE("Collision, failed to create collidable");
    }
}

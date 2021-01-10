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

namespace {

const std::map<std::string, Collideable::CollisionType> string_type_map {
    {"static", Collideable::CollisionType::STATIC},
    {"semi_solid", Collideable::CollisionType::SEMI_SOLID},
    {"slope", Collideable::CollisionType::SLOPE},
    {"player_hurtbox", Collideable::CollisionType::PLAYER_HURTBOX},
    {"player_hitbox", Collideable::CollisionType::PLAYER_HITBOX},
    {"enemy_hitbox", Collideable::CollisionType::ENEMY_HITBOX},
    {"transition", Collideable::CollisionType::TRANSITION},
    {"health", Collideable::CollisionType::HEALTH},
    {"collectible", Collideable::CollisionType::COLLECTIBLE},
    {"interactable", Collideable::CollisionType::INTERACTABLE},
};

} // namespace

Collision::Collision(std::weak_ptr<Transform> trans) :
    trans_(trans) {

    collideable_ = std::make_shared<CollideableStatic>(trans_);
}

void Collision::update() {
    collideable_->update();
}

std::shared_ptr<Collision> Collision::createFromJson(nlohmann::json j, std::weak_ptr<Transform> trans) {
    auto collision = std::make_shared<Collision>(trans);

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

void Collision::setCollideable(nlohmann::json j) {
    auto coll = Collideable::createFromJson(j, trans_);

    if (coll) {
        collideable_ = Collideable::createFromJson(j, trans_);
    } else {
        // This should never happen as createFromJson will throw
        // Message here for debug reasons
        LOGE("Collision, failed to create collidable");
    }
}

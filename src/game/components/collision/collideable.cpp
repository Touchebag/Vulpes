#include "collideable.h"

#include "utils/log.h"

#include "components/collision/collideables/collideable_player_hurtbox.h"
#include "components/collision/collideables/collideable_transition.h"
#include "components/collision/collideables/collideable_health.h"
#include "components/collision/collideables/collideable_collectible.h"
#include "components/collision/collideables/collideable_interactable.h"

#include "components/collision/collideables/movement/collideable_static.h"
#include "components/collision/collideables/movement/collideable_semisolid.h"
#include "components/collision/collideables/movement/collideable_slope.h"

#include "components/collision/collideables/damage/collideable_player_hitbox.h"
#include "components/collision/collideables/damage/collideable_player_dive.h"
#include "components/collision/collideables/damage/collideable_enemy_hitbox.h"

#include "collision_utils.h"

namespace {

const std::map<std::string, Collideable::CollisionType> string_type_map {
    {"static", Collideable::CollisionType::STATIC},
    {"semi_solid", Collideable::CollisionType::SEMI_SOLID},
    {"slope", Collideable::CollisionType::SLOPE},
    {"player_hurtbox", Collideable::CollisionType::PLAYER_HURTBOX},
    {"player_hitbox", Collideable::CollisionType::PLAYER_HITBOX},
    {"player_dive", Collideable::CollisionType::PLAYER_DIVE},
    {"enemy_hitbox", Collideable::CollisionType::ENEMY_HITBOX},
    {"transition", Collideable::CollisionType::TRANSITION},
    {"health", Collideable::CollisionType::HEALTH},
    {"collectible", Collideable::CollisionType::COLLECTIBLE},
    {"interactable", Collideable::CollisionType::INTERACTABLE},
};

} // namespace

Collideable::Collideable(std::weak_ptr<Transform> trans) :
    trans_(trans) {

    // TODO Should be 0?
    // Need to fix default collision not being clickable in editor
    setHitbox(50, 50);
}

void Collideable::update() {
}

void Collideable::setHitbox(int width, int height) {
    hbox_ = std::make_shared<Hitbox>(width, height);
}

std::shared_ptr<Collideable> Collideable::createFromJson(nlohmann::json j, std::weak_ptr<Transform> trans, std::weak_ptr<Actions> actions) {
    CollisionType type;
    if (j.contains("type")) {
        auto type_entry = string_type_map.find(j["type"].get<std::string>());
        if (type_entry != string_type_map.end()) {
            type = type_entry->second;
        } else {
            throw std::invalid_argument("Collideable: invalid type " + j["type"].get<std::string>());
        }
    } else {
        throw std::invalid_argument("Collideable: missing type");
    }

    switch (type) {
        case CollisionType::STATIC:
            {
                auto coll = std::make_shared<CollideableStatic>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::SEMI_SOLID:
            {
                auto coll = std::make_shared<CollideableSemiSolid>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::SLOPE:
            {
                auto coll = std::make_shared<CollideableSlope>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::PLAYER_HURTBOX:
            {
                auto coll = std::make_shared<CollideablePlayerHurtbox>(trans, actions);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::PLAYER_HITBOX:
            {
                auto coll = std::make_shared<CollideablePlayerHitbox>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::PLAYER_DIVE:
            {
                auto coll = std::make_shared<CollideablePlayerDive>(trans, actions);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::ENEMY_HITBOX:
            {
                auto coll = std::make_shared<CollideableEnemyHitbox>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::TRANSITION:
            {
                auto coll = std::make_shared<CollideableTransition>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::HEALTH:
            {
                auto coll = std::make_shared<CollideableHealth>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::COLLECTIBLE:
            {
                auto coll = std::make_shared<CollideableCollectible>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::INTERACTABLE:
            {
                auto coll = std::make_shared<CollideableInteractable>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        default:
            LOGW("Collideable: This should never happen");
            break;
    }

    return {};
}

void Collideable::reloadFromJson(nlohmann::json j) {
    int hitbox_width = j["width"];
    int hitbox_height = j["height"];

    hbox_ = std::make_shared<Hitbox>(hitbox_width, hitbox_height);

    if (j.contains("offset")) {
        if (j["offset"].contains("x") && j["offset"].contains("y")) {
            hbox_->setOffset({j["offset"]["x"].get<int>(), j["offset"]["y"].get<int>()});
        } else {
            LOGW("Collideable: offset requires x and y");
        }
    }
}

std::optional<nlohmann::json> Collideable::outputToJson() {
    nlohmann::json j;

    j["width"] = getHitbox()->width_;
    j["height"] = getHitbox()->height_;

    for (auto it : string_type_map) {
        if (it.second == getType()) {
            j["type"] = it.first;
            return j;
        }
    }

    throw std::runtime_error("Collideable: this should not happen");
}

bool Collideable::collides(std::shared_ptr<const Transform> other_trans, std::shared_ptr<const Hitbox> other_hbox) const {
    auto this_trans = getTransform().lock();

    if (this_trans && hbox_ && other_hbox && other_trans) {
        return collidesX(this_trans, hbox_, other_trans, other_hbox)
            && collidesY(this_trans, hbox_, other_trans, other_hbox);
    }

    return false;
}

bool Collideable::collides(std::weak_ptr<const Collideable> other_entity) const {
    if (auto other_ent = other_entity.lock()) {
        auto other_trans = other_ent->getTransform().lock();
        auto other_hbox = other_ent->getHitbox();

        return collides(other_trans, other_hbox);
    }

    return false;
}

const std::shared_ptr<const Hitbox> Collideable::getHitbox() const {
    return hbox_;
}

std::weak_ptr<const Transform> Collideable::getTransform() const {
    return trans_;
}

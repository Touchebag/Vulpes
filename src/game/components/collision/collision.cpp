#include "collision.h"

#include "utils/log.h"

#include "collision_player_hurtbox.h"
#include "collision_transition.h"
#include "collision_health.h"
#include "collision_collectible.h"
#include "collision_interactable.h"

#include "movement/collision_static.h"
#include "movement/collision_semisolid.h"

#include "damage/collision_player_hitbox.h"
#include "damage/collision_enemy_hitbox.h"

#include "collision_utils.h"

namespace {

const std::map<std::string, Collision::CollisionType> string_type_map {
    {"static", Collision::CollisionType::STATIC},
    {"semi_solid", Collision::CollisionType::SEMI_SOLID},
    {"player_hurtbox", Collision::CollisionType::PLAYER_HURTBOX},
    {"player_hitbox", Collision::CollisionType::PLAYER_HITBOX},
    {"enemy_hitbox", Collision::CollisionType::ENEMY_HITBOX},
    {"transition", Collision::CollisionType::TRANSITION},
    {"health", Collision::CollisionType::HEALTH},
    {"collectible", Collision::CollisionType::COLLECTIBLE},
    {"interactable", Collision::CollisionType::INTERACTABLE},
};

} // namespace

Collision::Collision(std::weak_ptr<Transform> trans) :
    trans_(trans) {

    // TODO Should be 0?
    // Need to fix default collision not being clickable in editor
    setHitbox(50, 50);
}

void Collision::update() {
}

void Collision::setHitbox(int width, int height) {
    hbox_ = std::make_shared<Hitbox>(width, height);
}

std::shared_ptr<Collision> Collision::createFromJson(nlohmann::json j, std::weak_ptr<Transform> trans) {
    CollisionType type;
    if (j.contains("type")) {
        auto type_entry = string_type_map.find(j["type"].get<std::string>());
        if (type_entry != string_type_map.end()) {
            type = type_entry->second;
        } else {
            throw std::invalid_argument("Collision: invalid type " + j["type"].get<std::string>());
        }
    } else {
        throw std::invalid_argument("Collision: missing type");
    }

    switch (type) {
        case CollisionType::STATIC:
            {
                auto coll = std::make_shared<CollisionStatic>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::SEMI_SOLID:
            {
                auto coll = std::make_shared<CollisionSemiSolid>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::PLAYER_HURTBOX:
            {
                auto coll = std::make_shared<CollisionPlayerHurtbox>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::PLAYER_HITBOX:
            {
                auto coll = std::make_shared<CollisionPlayerHitbox>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::ENEMY_HITBOX:
            {
                auto coll = std::make_shared<CollisionEnemyHitbox>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::TRANSITION:
            {
                auto coll = std::make_shared<CollisionTransition>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::HEALTH:
            {
                auto coll = std::make_shared<CollisionHealth>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::COLLECTIBLE:
            {
                auto coll = std::make_shared<CollisionCollectible>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::INTERACTABLE:
            {
                auto coll = std::make_shared<CollisionInteractable>(trans);
                coll->reloadFromJson(j);
                return coll;
            }
        default:
            LOGW("Collision: This should never happen");
            break;
    }

    return {};
}

void Collision::reloadFromJson(nlohmann::json j) {
    int hitbox_width = j["width"];
    int hitbox_height = j["height"];

    hbox_ = std::make_shared<Hitbox>(hitbox_width, hitbox_height);
}

std::optional<nlohmann::json> Collision::outputToJson() {
    nlohmann::json j;

    j["width"] = getHitbox()->width_;
    j["height"] = getHitbox()->height_;

    for (auto it : string_type_map) {
        if (it.second == getType()) {
            j["type"] = it.first;
            return j;
        }
    }

    throw std::runtime_error("Collision: this should not happen");
}

bool Collision::collides(std::weak_ptr<const Collision> other_entity) {
    if (auto other_ent = other_entity.lock()) {
        auto this_trans = trans_.lock();
        auto other_trans = other_ent->trans_.lock();
        auto other_hbox = other_ent->getHitbox();

        if (this_trans && hbox_ && other_trans && other_hbox) {
            return collidesX(this_trans, hbox_, other_trans, other_hbox)
                && collidesY(this_trans, hbox_, other_trans, other_hbox);
        }
    }

    return false;
}

const std::shared_ptr<const Hitbox> Collision::getHitbox() const {
    return hbox_;
}

std::weak_ptr<const Transform> Collision::getTransform() const {
    return trans_;
}

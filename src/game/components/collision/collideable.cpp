#include "collideable.h"

#include "utils/log.h"

#include "components/collision/collideables/collideable_transition.h"
#include "components/collision/collideables/collideable_collectible.h"
#include "components/collision/collideables/collideable_interactable.h"

#include "components/collision/collideables/movement/collideable_static.h"
#include "components/collision/collideables/movement/collideable_slope.h"

#include "components/collision/collideables/collideable_hitbox.h"
#include "components/collision/collideables/collideable_hurtbox.h"

#include "components/collision/collideables/collideable_sensor.h"

#include "collision_utils.h"

#include "components/component_store.h"

Collideable::Collideable(std::weak_ptr<ComponentStore> components) :
    components_(components) {

    // TODO Should be 0?
    // Need to fix default collision not being clickable in editor
    setHitbox(50, 50);
}

void Collideable::update() {
}

void Collideable::setHitbox(int width, int height) {
    hbox_ = std::make_shared<Hitbox>(width, height);
}

std::shared_ptr<Collideable> Collideable::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components) {
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
                auto coll = std::make_shared<CollideableStatic>(components);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::SLOPE:
            {
                auto coll = std::make_shared<CollideableSlope>(components);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::HURTBOX:
            {
                auto coll = std::make_shared<CollideableHurtbox>(components);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::HITBOX:
            {
                auto coll = std::make_shared<CollideableHitbox>(components);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::TRANSITION:
            {
                auto coll = std::make_shared<CollideableTransition>(components);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::COLLECTIBLE:
            {
                auto coll = std::make_shared<CollideableCollectible>(components);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::INTERACTABLE:
            {
                auto coll = std::make_shared<CollideableInteractable>(components);
                coll->reloadFromJson(j);
                return coll;
            }
        case CollisionType::SENSOR:
            {
                auto coll = std::make_shared<CollideableSensor>(components);
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

    teams_.clear();
    if (j.contains("teams")) {
        for (int team : j["teams"]) {
            teams_.insert(team);
        }
    }
}

std::optional<nlohmann::json> Collideable::outputToJson() {
    nlohmann::json j;

    j["width"] = getHitbox()->width_;
    j["height"] = getHitbox()->height_;

    if (!teams_.empty()) {
        j["teams"] = teams_;
    }

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

const std::set<int> Collideable::getTeams() const {
    return teams_;
}

std::weak_ptr<const Transform> Collideable::getTransform() const {
    if (auto comp = components_.lock()) {
        return comp->getComponent<Transform>();
    } else {
        return {};
    }
}

void Collideable::setDirectionMultiplier(int multiplier) {
    hbox_->direction_multiplier_ = multiplier;
}

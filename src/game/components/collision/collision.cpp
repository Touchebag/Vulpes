#include "collision.h"

#include "utils/log.h"

#include "collision_static.h"
#include "collision_player_hurtbox.h"
#include "collision_player_hitbox.h"
#include "collision_enemy_hitbox.h"
#include "collision_transition.h"
#include "collision_health.h"
#include "collision_collectible.h"

namespace {

int getAbsRight(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getX() + hbox->getRight();
}

int getAbsLeft(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getX() + hbox->getLeft();
}

int getAbsTop(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getY() + hbox->getTop();
}

int getAbsBottom(std::shared_ptr<const Transform> trans, std::shared_ptr<const Hitbox> hbox) {
    return trans->getY() + hbox->getBottom();
}

bool collidesX(std::shared_ptr<Transform> first_trans, std::shared_ptr<Hitbox> first_hbox,
               std::shared_ptr<Transform> second_trans, std::shared_ptr<Hitbox> second_hbox) {
    return getAbsLeft(first_trans, first_hbox) < getAbsRight(second_trans, second_hbox)
        && getAbsRight(first_trans, first_hbox) > getAbsLeft(second_trans, second_hbox);
}

bool collidesY(std::shared_ptr<Transform> first_trans, std::shared_ptr<Hitbox> first_hbox,
               std::shared_ptr<Transform> second_trans, std::shared_ptr<Hitbox> second_hbox) {
    return getAbsTop(first_trans, first_hbox) < getAbsBottom(second_trans, second_hbox)
        && getAbsBottom(first_trans, first_hbox) > getAbsTop(second_trans, second_hbox);
}

const std::map<std::string, Collision::CollisionType> string_type_map {
    {"static", Collision::CollisionType::STATIC},

    {"player_hurtbox", Collision::CollisionType::PLAYER_HURTBOX},

    {"player_hitbox", Collision::CollisionType::PLAYER_HITBOX},

    {"enemy_hitbox", Collision::CollisionType::ENEMY_HITBOX},

    {"transition", Collision::CollisionType::TRANSITION},

    {"health", Collision::CollisionType::HEALTH},

    {"collectible", Collision::CollisionType::COLLECTIBLE},
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

const std::shared_ptr<Hitbox>& Collision::getHitbox() const {
    return hbox_;
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

std::pair<double, double> Collision::getMaximumMovement(double stepX, double stepY,
        std::shared_ptr<const Collision> other_coll) {
    auto other_trans = other_coll->trans_.lock();
    auto other_hbox = other_coll->getHitbox();

    if (other_trans) {
        return getMaximumMovement(stepX, stepY, other_trans, other_hbox);
    } else {
        LOGW("Collision: unable to lock other");
        return {stepX, stepY};
    }
}

std::pair<double, double> Collision::getMaximumMovement(double stepX, double stepY, std::shared_ptr<Transform> other_trans, std::shared_ptr<Hitbox> other_hbox) {
    double retX = stepX, retY = stepY;


    if (auto this_trans = trans_.lock()) {
        // Check if collision after move
        std::shared_ptr<Transform> new_pos = std::make_shared<Transform>();
        new_pos->setPosition(this_trans->getX() + static_cast<int>(stepX), this_trans->getY() + static_cast<int>(stepY));

        bool collides_x = collidesX(new_pos, hbox_, other_trans, other_hbox);
        bool collides_y = collidesY(new_pos, hbox_, other_trans, other_hbox);

        if (!(collides_x && collides_y)) {
            return {retX, retY};
        }

        collides_x = collidesX(this_trans, hbox_, other_trans, other_hbox);
        collides_y = collidesY(this_trans, hbox_, other_trans, other_hbox);

        // If X direction was already colliding before movement then we are parallel in this direction
        // I.e. do not change speed
        if (!collides_x) {
            if (stepX > 0.0) {
                retX = std::min(stepX, static_cast<double>(getAbsLeft(other_trans, other_hbox) - getAbsRight(this_trans, hbox_)));
            } else if (stepX < 0.0) {
                retX = std::max(stepX, static_cast<double>(getAbsRight(other_trans, other_hbox) - getAbsLeft(this_trans, hbox_)));
            }
        }

        // Same for Y
        if (!collides_y) {
            if (stepY > 0.0) {
                retY = std::min(stepY, static_cast<double>(getAbsTop(other_trans, other_hbox) - getAbsBottom(this_trans, hbox_)));
            } else if (stepY < 0.0) {
                retY = std::max(stepY, static_cast<double>(getAbsBottom(other_trans, other_hbox) - getAbsTop(this_trans, hbox_)));
            }
        }
    } else {
        LOGW("Collision: Missing transform");
    }

    return {retX, retY};
}

const collision::AttackAttributes Collision::getAttributes() const {
    LOGW("Getting attack attributes on invalid object");
    return {};
}

std::weak_ptr<const Transform> Collision::getTransform() const {
    return trans_;
}

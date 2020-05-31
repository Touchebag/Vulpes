#include "collision.h"

#include "utils/log.h"

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

void Collision::reloadFromJson(nlohmann::json j) {
    if (j.contains("type")) {
        auto type_entry = string_type_map.find(j["type"].get<std::string>());
        if (type_entry != string_type_map.end()) {
            type_ = type_entry->second;
        } else {
            throw std::invalid_argument("Collision: invalid type" + j["type"].get<std::string>());
        }
    } else {
        throw std::invalid_argument("Collision: missing type");
    }

    int hitbox_width = j["width"];
    int hitbox_height = j["height"];

    if (j.contains("damage")) {
        damage_ = j["damage"];
        if (damage_ < 0) {
            LOGW("Damage must be positive but is %i, setting to 0", damage_);
            damage_ = 0;
        }
    }

    hbox_ = std::make_shared<Hitbox>(hitbox_width, hitbox_height);
}

std::optional<nlohmann::json> Collision::outputToJson() {
    nlohmann::json j;

    j["width"] = getHitbox()->width_;
    j["height"] = getHitbox()->height_;

    if (damage_ > 0) {
        j["damage"] = damage_;
    }

    for (auto it : string_type_map) {
        if (it.second == type_) {
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

    return getMaximumMovement(stepX, stepY, other_trans, other_hbox);
}

std::pair<double, double> Collision::getMaximumMovement(double stepX, double stepY, std::shared_ptr<Transform> other_trans, std::shared_ptr<Hitbox> other_hbox) {
    double retX = stepX, retY = stepY;

    auto this_trans = trans_.lock();

    if (this_trans) {
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
    }

    return {retX, retY};
}

Collision::CollisionType Collision::getType() const {
    return type_;
}

int Collision::getDamage() const {
    return damage_;
}

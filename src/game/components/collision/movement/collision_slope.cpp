#include "collision_slope.h"

#include "components/collision/collision_utils.h"
#include "utils/log.h"

CollisionSlope::CollisionSlope(std::weak_ptr<Transform> trans) :
    ICollisionMovement(trans) {
}

void CollisionSlope::recalculateConstants() {
    if (auto this_trans = trans_.lock()) {
        auto top = getAbsTop(this_trans, hbox_);
        auto bot = getAbsBottom(this_trans, hbox_);
        auto left = getAbsLeft(this_trans, hbox_);
        auto right = getAbsRight(this_trans, hbox_);

        // Y axis is flipped so upward (right) facing slopes will be negative
        if (direction_right_) {
            slope_coeff_ = static_cast<double>(top - bot) / (right - left);
            slope_const_ = bot - static_cast<int>(slope_coeff_ * left);
        } else {
            slope_coeff_ = static_cast<double>(bot - top) / (right - left);
            slope_const_ = top - static_cast<int>(slope_coeff_ * left);
        }
    }
}

void CollisionSlope::reloadFromJson(nlohmann::json j) {
    Collision::reloadFromJson(j);

    if (j.contains("direction")) {
        auto str = j["direction"].get<std::string>();

        if (str == "right") {
            direction_right_ = true;
        } else if (str == "left") {
            direction_right_ = false;
        } else {
            LOGD("Slope: missing direction, defaulting to right");
        }
    } else {
        LOGD("Slope: missing direction, defaulting to right");
    }

    recalculateConstants();
}

std::optional<nlohmann::json> CollisionSlope::outputToJson() {
    nlohmann::json j = Collision::outputToJson().value();

    j["direction"] = direction_right_ ? "right" : "left";

    return j;
}

Collision::CollisionType CollisionSlope::getType() const {
    return Collision::CollisionType::SLOPE;
}

int CollisionSlope::getCurrentHeight(int x) const {
    auto ret_x = static_cast<int>(x * slope_coeff_) + slope_const_;

    return ret_x;
}

// TODO Some duplicated code with CollisionStatic
// See if can be broken out
std::pair<double, double> CollisionSlope::getMaximumMovement(double stepX, double stepY,
        std::shared_ptr<const Collision> other_coll) const {
    auto other_trans = other_coll->getTransform().lock();
    auto other_hbox = other_coll->getHitbox();

    if (!other_trans) {
        LOGW("CollisionSlope: unable to lock other");
        return {stepX, stepY};
    }

    double retX = stepX, retY = stepY;

    if (auto this_trans = trans_.lock()) {
        // Check if collision after move
        std::shared_ptr<Transform> new_pos = std::make_shared<Transform>();
        new_pos->setPosition(other_trans->getX() + static_cast<int>(stepX), other_trans->getY() + static_cast<int>(stepY));

        bool collides_x = collidesX(new_pos, other_hbox, this_trans, hbox_);
        bool collides_y = collidesY(new_pos, other_hbox, this_trans, hbox_);

        if (!(collides_x && collides_y)) {
            return {retX, retY};
        }

        collides_x = collidesX(other_trans, other_hbox, this_trans, hbox_);
        collides_y = collidesY(other_trans, other_hbox, this_trans, hbox_);

        // Current position needed to calculate relative movement for snapping
        int entity_current_ground_pos = other_trans->getY() + (other_hbox->height_ / 2);
        int entity_new_ground_pos = new_pos->getY() + (other_hbox->height_ / 2);
        int current_height = 0;

        // Clamp height at edges
        int entity_x_pos = other_trans->getX();
        if (entity_x_pos < getAbsLeft(this_trans, hbox_)) {
            current_height = direction_right_ ?
                                 getAbsBottom(this_trans, hbox_) :
                                 getAbsTop(this_trans, hbox_);
        } else if (entity_x_pos > getAbsRight(this_trans, hbox_)) {
            current_height = direction_right_ ?
                                 getAbsTop(this_trans, hbox_) :
                                 getAbsBottom(this_trans, hbox_);
        } else {
            current_height = getCurrentHeight(static_cast<int>(other_trans->getX() + retX));
        }

        // If new position is just above slope surface, snap downwards
        if (entity_new_ground_pos + 20 > current_height) {
            return {retX, current_height - entity_current_ground_pos};
        } else {
            return {retX, retY};
        }
    } else {
        LOGW("CollisionSlope: Missing transform");
    }

    return {retX, retY};
}

#include "collideable_slope.h"

#include "components/collision/collision_utils.h"
#include "utils/log.h"

CollideableSlope::CollideableSlope(std::weak_ptr<Transform> trans) :
    ICollideableMovement(trans) {
}

void CollideableSlope::recalculateConstants() {
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

void CollideableSlope::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

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

std::optional<nlohmann::json> CollideableSlope::outputToJson() {
    nlohmann::json j = Collideable::outputToJson().value();

    j["direction"] = direction_right_ ? "right" : "left";

    return j;
}

Collideable::CollisionType CollideableSlope::getType() const {
    return Collideable::CollisionType::SLOPE;
}

int CollideableSlope::getCurrentHeight(int x) const {
    auto ret_x = static_cast<int>(x * slope_coeff_) + slope_const_;

    return ret_x;
}

// TODO Some duplicated code with CollideableStatic
// See if can be broken out
std::pair<double, double> CollideableSlope::getMaximumMovement(double stepX, double stepY,
        std::shared_ptr<const Collideable> other_coll) const {
    auto other_trans = other_coll->getTransform().lock();
    auto other_hbox = other_coll->getHitbox();

    if (!other_trans) {
        LOGW("CollideableSlope: unable to lock other");
        return {stepX, stepY};
    }

    double retX = stepX, retY = stepY;

    if (auto this_trans = trans_.lock()) {
        // Check if collision after move
        auto collision_data = calculateSweptCollision(other_coll, {stepX, stepY});

        // Calculate slope even if colliding before
        if (collision_data.collision_time >= 1.0) {
            return {retX, retY};
        }

        std::shared_ptr<Transform> new_pos = std::make_shared<Transform>(std::weak_ptr<ComponentStore>({}));
        new_pos->setPosition(other_trans->getX() + static_cast<int>(stepX), other_trans->getY() + static_cast<int>(stepY));

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
        // but only if we are moving down (e.g. not if jumping up slope)
        if (entity_new_ground_pos + 20 > current_height && stepY >= 0) {
            return {retX, current_height - entity_current_ground_pos};
        } else {
            return {retX, retY};
        }
    } else {
        LOGW("CollideableSlope: Missing transform");
    }

    return {retX, retY};
}

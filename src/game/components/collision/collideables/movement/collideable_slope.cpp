#include "collideable_slope.h"

#include "components/collision/collision_utils.h"
#include "utils/log.h"

CollideableSlope::CollideableSlope(std::weak_ptr<ComponentStore> components) :
    ICollideableMovement(components) {
}

void CollideableSlope::recalculateConstants() {
    if (auto hbox = getHitbox()) {
        slope_coeff_ = static_cast<double>(hbox->height_) / static_cast<double>(hbox->width_);
        slope_coeff_ *= direction_right_ ? -1 : 1;
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
            LOGD("Slope: invalid direction, defaulting to right");
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

double CollideableSlope::getCurrentHeight(double x, std::shared_ptr<const Transform> this_trans) const {
    // Normalise around zero
    double x0 = x - this_trans->getX();
    // Calculate y distance from zero
    double h0 = x0 * slope_coeff_;
    // Translate back to absolute values, clamping at top
    double abs_h = std::max(h0 + this_trans->getY(), static_cast<double>(getAbsTop(this_trans, hbox_)));

    return abs_h;
}

std::pair<double, double> CollideableSlope::getMaximumMovement(double stepX, double stepY,
        std::shared_ptr<const Collideable> other_coll) const {
    auto other_trans = other_coll->getTransform().lock();
    auto other_hbox = other_coll->getHitbox();

    if (!other_trans) {
        LOGW("CollideableSlope: unable to lock other");
        return {stepX, stepY};
    }

    double retX = stepX, retY = stepY;

    if (auto this_trans = getTransform().lock()) {
        // Check if collision after move
        auto collision_data = calculateSweptCollision(other_coll, {stepX, stepY});

        // Calculate slope even if colliding before
        if (collision_data.collision_time >= 1.0) {
            return {retX, retY};
        }

        std::shared_ptr<Transform> new_pos = std::make_shared<Transform>(std::weak_ptr<ComponentStore>({}));
        new_pos->setPosition(other_trans->getX() + static_cast<int>(stepX), other_trans->getY() + static_cast<int>(stepY));

        // Current position needed to calculate relative movement for snapping
        int entity_current_ground_pos = other_trans->getY() + static_cast<int>(other_hbox->height_ / 2);
        int entity_new_ground_pos = new_pos->getY() + static_cast<int>(other_hbox->height_ / 2);

        int current_height = static_cast<int>(getCurrentHeight(other_trans->getX() + retX, this_trans));

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

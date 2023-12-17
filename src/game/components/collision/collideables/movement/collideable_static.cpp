#include "collideable_static.h"

#include "components/collision/collision_utils.h"
#include "utils/log.h"

CollideableStatic::CollideableStatic(std::weak_ptr<ComponentStore> components) :
    ICollideableMovement(components) {
}

void CollideableStatic::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

    // Reset all in case we reload exisisting
    solid_top_ = true;
    solid_bottom_ = true;
    solid_left_ = true;
    solid_right_ = true;

    if (j.contains("open_sides")) {
        for (auto it : j["open_sides"]) {
            auto str = it.get<std::string>();

            if (str == "top") {
                solid_top_ = false;
            } else if (str == "bottom") {
                solid_bottom_ = false;
            } else if (str == "left") {
                solid_left_ = false;
            } else if (str == "right") {
                solid_right_ = false;
            }
        }
    }
}

std::optional<nlohmann::json> CollideableStatic::outputToJson() {
    auto j_opt = Collideable::outputToJson();

    if (!j_opt) {
        return j_opt;
    }

    auto j = j_opt.value();
    std::vector<std::string> open_sides;

    if (!solid_top_) {
        open_sides.push_back("top");
    }
    if (!solid_bottom_) {
        open_sides.push_back("bottom");
    }
    if (!solid_right_) {
        open_sides.push_back("right");
    }
    if (!solid_left_) {
        open_sides.push_back("left");
    }

    if (!open_sides.empty()) {
        j["open_sides"] = open_sides;
    }

    return {j};
}

Collideable::CollisionType CollideableStatic::getType() const {
    return Collideable::CollisionType::STATIC;
}

std::pair<double, double> CollideableStatic::getMaximumMovement(double stepX, double stepY,
        std::shared_ptr<const Collideable> other_coll) const {
    auto other_trans = other_coll->getTransform().lock();
    auto other_hbox = other_coll->getHitbox();

    if (!other_trans) {
        LOGW("CollideableStatic: unable to lock other");
        return {stepX, stepY};
    }

    double retX = stepX, retY = stepY;

    if (auto this_trans = getTransform().lock()) {
        auto collision_data = calculateSweptCollision(other_coll, {stepX, stepY});

        if (collision_data.collision_time < 0.0 || collision_data.collision_time >= 1.0) {
            return {retX, retY};
        }

        auto collides_x_old = collidesX(other_trans, other_hbox, this_trans, hbox_);
        auto collides_y_old = collidesY(other_trans, other_hbox, this_trans, hbox_);

        // Check Y first to allow leniency when colliding in corners
        if (!collides_y_old) {
            if ((collision_data.normals.second == -1 && solid_top_)
             || (collision_data.normals.second == 1 && solid_bottom_)) {
                retY = stepY * collision_data.collision_time;
            }
        }

        if (!collides_x_old) {
            if ((collision_data.normals.first == -1 && solid_left_)
             || (collision_data.normals.first == 1 && solid_right_)) {
                retX = stepX * collision_data.collision_time;
            }
        }
    } else {
        LOGW("CollideableStatic: Missing transform");
    }

    return {retX, retY};
}

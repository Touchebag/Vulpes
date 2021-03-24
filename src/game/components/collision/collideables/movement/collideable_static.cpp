#include "collideable_static.h"

#include "components/collision/collision_utils.h"
#include "utils/log.h"

CollideableStatic::CollideableStatic(std::weak_ptr<Transform> trans) :
    ICollideableMovement(trans) {
}

Collideable::CollisionType CollideableStatic::getType() const {
    return Collideable::CollisionType::STATIC;
}

// TODO Mostly duplicated code with CollideableSemiSolid
// See if can be broken out
std::pair<double, double> CollideableStatic::getMaximumMovement(double stepX, double stepY,
        std::shared_ptr<const Collideable> other_coll) const {
    auto other_trans = other_coll->getTransform().lock();
    auto other_hbox = other_coll->getHitbox();

    if (!other_trans) {
        LOGW("CollideableStatic: unable to lock other");
        return {stepX, stepY};
    }

    double retX = stepX, retY = stepY;

    if (auto this_trans = trans_.lock()) {
        auto collision_data = calculateSweptCollision(other_coll, {stepX, stepY});

        if (collision_data.collision_time < 0.0 || collision_data.collision_time >= 1.0) {
            return {retX, retY};
        }

        auto collides_x_old = collidesX(other_trans, other_hbox, this_trans, hbox_);
        auto collides_y_old = collidesY(other_trans, other_hbox, this_trans, hbox_);

        // Check Y first to allow leniency when collidiong in corners
        if (collision_data.normals.second != 0 && !collides_y_old) {
            retY = stepY * collision_data.collision_time;
        }

        if (collision_data.normals.first != 0 && !collides_x_old) {
            retX = stepX * collision_data.collision_time;
        }
    } else {
        LOGW("CollideableStatic: Missing transform");
    }

    return {retX, retY};
}

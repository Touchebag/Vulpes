#include "collideable_semisolid.h"

#include "components/collision/collision_utils.h"
#include "utils/log.h"

CollideableSemiSolid::CollideableSemiSolid(std::weak_ptr<ComponentStore> components) :
    ICollideableMovement(components) {
}

Collideable::CollisionType CollideableSemiSolid::getType() const {
    return Collideable::CollisionType::SEMI_SOLID;
}

// TODO Mostly duplicated code with CollideableStatic
// See if can be broken out
std::pair<double, double> CollideableSemiSolid::getMaximumMovement(double stepX, double stepY,
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

        auto collides_y_old = collidesY(other_trans, other_hbox, this_trans, hbox_);

        // Only block if collides from above
        if (collision_data.normals.second == -1 && !collides_y_old) {
            retY = stepY * collision_data.collision_time;
        }
    } else {
        LOGW("CollideableStatic: Missing transform");
    }

    return {retX, retY};
}

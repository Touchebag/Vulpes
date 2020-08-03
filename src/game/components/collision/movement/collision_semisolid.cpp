#include "collision_semisolid.h"

#include "components/collision/collision_utils.h"
#include "utils/log.h"

CollisionSemiSolid::CollisionSemiSolid(std::weak_ptr<Transform> trans) :
    ICollisionMovement(trans) {
}

Collision::CollisionType CollisionSemiSolid::getType() const {
    return Collision::CollisionType::SEMI_SOLID;
}

// TODO Mostly duplicated code with CollisionStatic
// See if can be broken out
std::pair<double, double> CollisionSemiSolid::getMaximumMovement(double stepX, double stepY,
        std::shared_ptr<const Collision> other_coll) const {
    auto other_trans = other_coll->getTransform().lock();
    auto other_hbox = other_coll->getHitbox();

    if (!other_trans) {
        LOGW("CollisionStatic: unable to lock other");
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

        // If entering from below, return early
        // Only check for collisions from above
        if (other_trans->getY() > this_trans->getY()) {
            return {retX, retY};
        }

        // If Y direction was already colliding before movement then we are parallel in this direction
        // I.e. for semisolid we should just return early to allow entering from the side
        if (!collides_y) {
            if (stepY > 0.0) {
                retY = std::min(stepY, static_cast<double>(getAbsTop(this_trans, hbox_) - getAbsBottom(other_trans, other_hbox)));
            } else if (stepY < 0.0) {
                retY = std::max(stepY, static_cast<double>(getAbsBottom(this_trans, hbox_) - getAbsTop(other_trans, other_hbox)));
            }
        } else {
            return {retX, retY};
        }

        // If X direction was already colliding before movement then we are parallel in this direction
        // I.e. do not change speed
        if (!collides_x) {
            if (stepX > 0.0) {
                retX = std::min(stepX, static_cast<double>(getAbsLeft(this_trans, hbox_) - getAbsRight(other_trans, other_hbox)));
            } else if (stepX < 0.0) {
                retX = std::max(stepX, static_cast<double>(getAbsRight(this_trans, hbox_) - getAbsLeft(other_trans, other_hbox)));
            }
        }
    } else {
        LOGW("CollisionStatic: Missing transform");
    }

    return {retX, retY};
}

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
        // Check if collision after move
        std::shared_ptr<Transform> new_pos = std::make_shared<Transform>();
        new_pos->setPosition(other_trans->getX() + static_cast<int>(stepX), other_trans->getY() + static_cast<int>(stepY));

        bool collides_x_new = collidesX(new_pos, other_hbox, this_trans, hbox_);
        bool collides_y_new = collidesY(new_pos, other_hbox, this_trans, hbox_);

        if (!(collides_x_new && collides_y_new)) {
            return {retX, retY};
        }

        auto collides_x_old = collidesX(other_trans, other_hbox, this_trans, hbox_);
        auto collides_y_old = collidesY(other_trans, other_hbox, this_trans, hbox_);

        // If Y direction was already colliding before movement then we are parallel in this direction
        // I.e. do not change speed
        if (!collides_y_old) {
            if (stepY > 0.0) {
                retY = std::min(stepY, static_cast<double>(getAbsTop(this_trans, hbox_) - getAbsBottom(other_trans, other_hbox)));
            } else if (stepY < 0.0) {
                retY = std::max(stepY, static_cast<double>(getAbsBottom(this_trans, hbox_) - getAbsTop(other_trans, other_hbox)));
            }
        }

        // Check if still colliding after y velocity change
        // This will prevent getting stuck at corners if entering diagonally
        new_pos->setPosition(other_trans->getX() + static_cast<int>(retX), other_trans->getY() + static_cast<int>(retY));
        collides_y_new = collidesY(new_pos, other_hbox, this_trans, hbox_);

        if (!(collides_x_new && collides_y_new)) {
            return {retX, retY};
        }

        // Same for X
        if (!collides_x_old) {
            if (stepX > 0.0) {
                retX = std::min(stepX, static_cast<double>(getAbsLeft(this_trans, hbox_) - getAbsRight(other_trans, other_hbox)));
            } else if (stepX < 0.0) {
                retX = std::max(stepX, static_cast<double>(getAbsRight(this_trans, hbox_) - getAbsLeft(other_trans, other_hbox)));
            }
        }
    } else {
        LOGW("CollideableStatic: Missing transform");
    }

    return {retX, retY};
}

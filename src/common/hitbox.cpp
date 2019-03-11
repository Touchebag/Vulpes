#include "hitbox.h"

void Hitbox::setHitbox(util::Right right, util::Left left, util::Top top, util::Bottom bottom) {
    right_ = right;
    left_ = left;
    top_ = top;
    bottom_ = bottom;
}

bool Hitbox::collides(Hitbox& hitbox) {
    return collidesX(hitbox) && collidesY(hitbox);
}

std::tuple<util::X, util::Y> Hitbox::getMaximumMovement(util::X stepX, util::Y stepY, Hitbox otherHitbox) {
    int retX = stepX, retY = stepY;

    // If X direction was already colliding last step then we are parallel in this direction
    // I.e. do no change speed
    if (!collidesX(otherHitbox)) {
        if (stepX > 0) {
            retX -= right_ + stepX - otherHitbox.left_;
        } else if (stepX < 0) {
            retX += otherHitbox.right_ - (left_ + stepX);
        }
    }

    // Same for Y
    if (!collidesY(otherHitbox)) {
        if (stepY > 0) {
            retY -= bottom_ + stepY - otherHitbox.top_;
        } else if (stepY < 0) {
            retY += otherHitbox.bottom_ - (top_ + stepY);
        }
    }

    return { util::X(retX), util::Y(retY) };
}

bool Hitbox::collidesX(Hitbox& otherHitbox) {
    return left_ < otherHitbox.right_ && right_ > otherHitbox.left_;
}

bool Hitbox::collidesY(Hitbox& otherHitbox) {
    return top_ < otherHitbox.bottom_ && bottom_ > otherHitbox.top_;
}

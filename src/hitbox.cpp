#include "hitbox.h"

void Hitbox::setHitbox(float right, float left, float top, float bottom) {
    right_ = right;
    left_ = left;
    top_ = top;
    bottom_ = bottom;
}

bool Hitbox::collides(const Hitbox& hitbox) {
    return collidesX(hitbox) && collidesY(hitbox);
}

std::tuple<util::X, util::Y> Hitbox::getMaximumMovement(util::X stepX, util::Y stepY, Hitbox otherHitbox) {
    float retX = stepX, retY = stepY;

    // If X direction was already colliding last step then we are parallell in this direction
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

bool Hitbox::collidesX(const Hitbox& otherHitbox) {
    return left_ < otherHitbox.right_ && right_ > otherHitbox.left_;
}

bool Hitbox::collidesY(const Hitbox& otherHitbox) {
    return top_ < otherHitbox.bottom_ && bottom_ > otherHitbox.top_;
}

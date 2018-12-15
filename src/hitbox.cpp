#include "hitbox.h"

Hitbox::setHitbox(float right, float left, float top, float bottom) {
    right_ = right;
    left_ = left;
    top_ = top;
    bottom_ = bottom;
}

bool Hitbox::collides(Hitbox hitbox) {
    if (left_ < hitbox.right_ && right_ > hitbox.left_ &&
            top_ > hitbox.bottom_ && bottom_ < hitbox.top_) {
        return true;
    } else {
        return false;
    }
}


#include "movable_entity.h"
#include "world.h"

void MovableEntity::move(util::X velX, util::Y velY) {
}

std::pair<util::X, util::Y> MovableEntity::getMaximumMovement(util::X velX, util::Y velY, Hitbox abs_hitbox) {
    int x = velX;
    int y = velY;

    World& worldInst = World::getInstance();

    Hitbox previous_abs = abs_hitbox;
    abs_hitbox.left_ += x;
    abs_hitbox.right_ += x;
    abs_hitbox.top_ += y;
    abs_hitbox.bottom_ += y;

    for (auto it = worldInst.getWorldObjects().begin(); it != worldInst.getWorldObjects().end(); ++it) {
        Hitbox other_hitbox = (*it)->getAbsHitbox();

        if (other_hitbox.collides(abs_hitbox)) {
            std::tuple<int, int> newMoveValues = previous_abs.getMaximumMovement(util::X(x), util::Y(y), other_hitbox);
            x = std::get<0>(newMoveValues);
            y = std::get<1>(newMoveValues);

            // Readjust abs_hitbox to new values
            abs_hitbox.left_ += x - velx_;
            abs_hitbox.right_ += x - velx_;
            abs_hitbox.top_ += y - vely_;
            abs_hitbox.bottom_ += y - vely_;
        }
    }

    return {util::X(x), util::Y(y)};
}

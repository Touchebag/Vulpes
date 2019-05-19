#include "movable.h"
#include "world.h"

void MovableEntity::move(util::X velX, util::Y velY, Transform& trans) {
    vely_ = velY;
    velx_ = velX;

    trans.move(velx_, vely_);
}

std::pair<util::X, util::Y> MovableEntity::getMaximumMovement(util::X velX, util::Y velY, Hitbox abs_hitbox) {
    int x = velX;
    int y = velY;

    World& worldInst = World::getInstance();

    Hitbox previous_abs = abs_hitbox;
    abs_hitbox.moveOffset({util::X(x), util::Y(y)});

    for (auto it = worldInst.getWorldObjects().begin(); it != worldInst.getWorldObjects().end(); ++it) {
        Hitbox other_hitbox = (*it)->getAbsHitbox();

        if (other_hitbox.collides(abs_hitbox)) {
            std::tuple<int, int> newMoveValues = previous_abs.getMaximumMovement(util::X(x), util::Y(y), other_hitbox);
            x = std::get<0>(newMoveValues);
            y = std::get<1>(newMoveValues);

            // Readjust abs_hitbox to new values
            abs_hitbox.moveOffset({util::X(x - velx_), util::Y(y - vely_)});
        }
    }

    return {util::X(x), util::Y(y)};
}
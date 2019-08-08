#include "movable.h"
#include "world.h"

MovableEntity::MovableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<Hitbox> hbox) :
    trans_(trans),
    hbox_(hbox) {
}

void MovableEntity::move(double velX, double velY) {
    if (auto trans = trans_.lock()) {
        vely_ = velY;
        velx_ = velX;

        int x = trans->getX();
        int y = trans->getY();
        trans->setPosition(x + static_cast<int>(velx_), y + static_cast<int>(vely_));
    }
}

std::pair<double, double> MovableEntity::getMaximumMovement(double velX, double velY, Hitbox abs_hitbox) {
    double x = static_cast<double>(velX);
    double y = static_cast<double>(velY);

    World& worldInst = World::getInstance();

    Hitbox previous_abs = abs_hitbox;
    abs_hitbox.moveOffset({x, y});

    for (auto it = worldInst.getWorldObjects().begin(); it != worldInst.getWorldObjects().end(); ++it) {
        Hitbox other_hitbox = (*it)->getAbsHitbox();

        if (other_hitbox.collides(abs_hitbox)) {
            std::tuple<double, double> newMoveValues = previous_abs.getMaximumMovement(x, y, other_hitbox);
            x = std::get<0>(newMoveValues);
            y = std::get<1>(newMoveValues);

            // Readjust abs_hitbox to new values
            abs_hitbox.moveOffset({x - static_cast<double>(velx_), y - static_cast<double>(vely_)});
        }
    }

    return {x, y};
}

double MovableEntity::getVelX() {
    return velx_;
}

double MovableEntity::getVelY() {
    return vely_;
}

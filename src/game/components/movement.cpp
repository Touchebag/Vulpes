#include "movement.h"
#include "system/world.h"

MovableEntity::MovableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<Hitbox> hbox, std::weak_ptr<Collision> collision) :
    trans_(trans),
    hbox_(hbox),
    collision_(collision) {
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

void MovableEntity::loadFromJson(nlohmann::json j) {
}

std::optional<nlohmann::json> MovableEntity::outputToJson() {
    nlohmann::json j;
    return j;
}

// TODO Store collidables separately in world
std::pair<double, double> MovableEntity::getMaximumMovement(double velX, double velY) {
    double x = velX;
    double y = velY;

    auto worldInst = World::getInstance<World::IWorldRead>();
    if (auto coll = collision_.lock()) {
        for (auto it = worldInst.getCollisions().begin(); it != worldInst.getCollisions().end(); ++it) {
            auto other_coll = (*it).lock();

            if (other_coll) {
                std::pair<double, double> newMoveValues = coll->getMaximumMovement(x, y, other_coll);
                x = newMoveValues.first;
                y = newMoveValues.second;
            }
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

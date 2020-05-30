#include "movement.h"
#include "system/world.h"

MovableEntity::MovableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<Collision> collision) :
    trans_(trans),
    collision_(collision) {
}

void MovableEntity::update() {
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

void MovableEntity::reloadFromJson(nlohmann::json /* j */) {
}

std::optional<nlohmann::json> MovableEntity::outputToJson() {
    nlohmann::json j;
    return j;
}

std::pair<double, double> MovableEntity::getMaximumMovement(double velX, double velY) {
    double x = velX;
    double y = velY;

    if (auto coll = collision_.lock()) {
        auto world_colls = World::getInstance<World::IWorldRead>().getCollisions(Collision::CollisionType::STATIC);
        for (auto it = world_colls.begin(); it != world_colls.end(); ++it) {
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

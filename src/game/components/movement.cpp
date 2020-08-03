#include "movement.h"

#include "collision/movement/i_collision_movement.h"
#include "system/world.h"
#include "utils/log.h"

namespace {

std::pair<double, double> checkMovement(double velX, double velY,
        std::shared_ptr<Collision> this_coll,
        Collision::CollisionType type) {
    double x = velX;
    double y = velY;

    auto world_colls = World::getInstance<World::IWorldRead>().getCollisions(type);
    for (auto it = world_colls.begin(); it != world_colls.end(); ++it) {
        auto other_coll = (*it).lock();

        if (other_coll) {
            if (auto static_coll = std::dynamic_pointer_cast<const ICollisionMovement>(other_coll)) {
                std::pair<double, double> newMoveValues = static_coll->getMaximumMovement(x, y, this_coll);
                x = newMoveValues.first;
                y = newMoveValues.second;
            }
        }
    }

    return {x, y};
}

} // namespace

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
    } else {
        LOGW("Movable: Missing transform");
    }
}

void MovableEntity::reloadFromJson(nlohmann::json /* j */) {
}

std::optional<nlohmann::json> MovableEntity::outputToJson() {
    nlohmann::json j;
    return j;
}

std::pair<double, double> MovableEntity::getMaximumMovement(double velX, double velY) {
    std::pair<double, double> vel = {velX, velY};

    if (auto coll = collision_.lock()) {
        vel = checkMovement(vel.first, vel.second, coll, Collision::CollisionType::STATIC);
        vel = checkMovement(vel.first, vel.second, coll, Collision::CollisionType::SEMI_SOLID);
    }

    return {vel.first, vel.second};
}

double MovableEntity::getVelX() {
    return velx_;
}

double MovableEntity::getVelY() {
    return vely_;
}

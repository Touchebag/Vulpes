#include "movement.h"

#include "collision/collideables/movement/i_collideable_movement.h"
#include "system/world.h"
#include "utils/log.h"

namespace {

std::pair<double, double> checkMovement(double velX, double velY,
        std::shared_ptr<Collision> this_coll,
        Collideable::CollisionType type) {
    double x = velX;
    double y = velY;

    auto world_colls = World::getInstance<World::IWorldRead>().getCollideables(type);
    for (auto it = world_colls.begin(); it != world_colls.end(); ++it) {
        auto other_coll = (*it).lock();

        if (other_coll) {
            if (auto static_coll = std::dynamic_pointer_cast<const ICollideableMovement>(other_coll)) {
                std::pair<double, double> newMoveValues = static_coll->getMaximumMovement(x, y, this_coll->getCollideable());
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
        vel = checkMovement(vel.first, vel.second, coll, Collideable::CollisionType::SLOPE);

        move_attr_.on_ground = vel.second != velY;

        vel = checkMovement(vel.first, vel.second, coll, Collideable::CollisionType::STATIC);
        vel = checkMovement(vel.first, vel.second, coll, Collideable::CollisionType::SEMI_SOLID);

        // If already on ground from slope, don't change
        move_attr_.on_ground = move_attr_.on_ground || (vel.second < velY);
        move_attr_.touching_wall = vel.first != velX;

        move_attr_.falling = vel.second > 0.0;
    }

    return {vel.first, vel.second};
}

double MovableEntity::getVelX() {
    return velx_;
}

double MovableEntity::getVelY() {
    return vely_;
}

const MovableEntity::MovementAttributes& MovableEntity::getMovementAttributes() {
    return move_attr_;
}

#include "movement.h"

#include "collision/collideables/movement/i_collideable_movement.h"
#include "system/world.h"
#include "utils/log.h"

namespace {

void recalculateTempCollision(std::shared_ptr<Transform> new_trans,
                              std::shared_ptr<Hitbox> new_hbox,
                              std::shared_ptr<const Transform> old_trans,
                              std::shared_ptr<const Hitbox> old_hbox,
                              double velX,
                              double velY) {
    new_hbox->width_ = old_hbox->width_ + abs(static_cast<int>(lround(velX))),
    new_hbox->height_ = old_hbox->height_ + abs(static_cast<int>(lround(velY))),
    new_trans->setPosition(old_trans->getX() + static_cast<int>(lround(velX / 2.0)),
                           old_trans->getY() + static_cast<int>(lround(velY / 2.0)));
}

std::pair<double, double> checkMovement(double velX, double velY,
        std::shared_ptr<Collision> this_coll,
        Collideable::CollisionType type) {
    double x = velX;
    double y = velY;

    // For broad sweep
    auto this_trans = this_coll->getTransform().lock();
    auto this_hbox = this_coll->getCollideable()->getHitbox();

    if (!(this_trans && this_hbox)) {
        return {x, y};
    }

    auto temp_trans = std::make_shared<Transform>(std::weak_ptr<ComponentStore>({}));
    auto temp_hitbox = std::make_shared<Hitbox>(0, 0);

    recalculateTempCollision(temp_trans, temp_hitbox, this_trans, this_hbox, x, y);

    auto world_colls = World::getInstance<World::IWorldRead>().getCollideables(type);
    for (auto it = world_colls.begin(); it != world_colls.end(); ++it) {
        auto other_coll = (*it).lock();

        if (other_coll) {
            // Broad sweep
            if (other_coll->collides(temp_trans, temp_hitbox)) {
                if (auto movement_coll = std::dynamic_pointer_cast<const ICollideableMovement>(other_coll)) {
                    std::pair<double, double> newMoveValues = movement_coll->getMaximumMovement(x, y, this_coll->getCollideable());
                    x = newMoveValues.first;
                    y = newMoveValues.second;

                    // Recalculate with new movement values
                    recalculateTempCollision(temp_trans, temp_hitbox, this_trans, this_hbox, x, y);
                }
            }
        }
    }

    return {x, y};
}

} // namespace

MovableEntity::MovableEntity(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void MovableEntity::update() {
}

void MovableEntity::move(double velX, double velY) {
    auto max_move = getMaximumMovement(velX, velY);

    if (auto trans = component_store_.lock()->transform) {
        velx_ = max_move.first;
        vely_ = max_move.second;

        int x = trans->getX();
        int y = trans->getY();
        trans->setPosition(x + static_cast<int>(velx_), y + static_cast<int>(vely_));

        if (getMovementAttributes().on_slope) {
            // Reset y velocity if adjusted by slope to avoid overshooting next frame
            vely_ = 0;
        }
    } else {
        LOGW("Movable: Missing transform");
    }
}

std::shared_ptr<MovableEntity> MovableEntity::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<MovableEntity>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void MovableEntity::reloadFromJson(nlohmann::json /* j */, File /* file_instance */) {
}

std::optional<nlohmann::json> MovableEntity::outputToJson() {
    nlohmann::json j;
    return j;
}

std::pair<double, double> MovableEntity::getMaximumMovement(double velX, double velY) {
    std::pair<double, double> vel = {velX, velY};

    if (auto coll = component_store_.lock()->collision) {
        vel = checkMovement(vel.first, vel.second, coll, Collideable::CollisionType::SLOPE);

        move_attr_.on_slope = vel.second != velY;

        vel = checkMovement(vel.first, vel.second, coll, Collideable::CollisionType::STATIC);
        vel = checkMovement(vel.first, vel.second, coll, Collideable::CollisionType::SEMI_SOLID);

        // If already on ground from slope, don't change
        move_attr_.on_ground = move_attr_.on_slope || vel.second < velY;
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

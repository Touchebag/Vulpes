#include "i_collideable_movement.h"

#include "components/collision/collision_utils.h"
#include "utils/log.h"

ICollideableMovement::ICollideableMovement(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
}

ICollideableMovement::CollisionData ICollideableMovement::calculateSweptCollision(std::weak_ptr<const Collideable> other_entity, const std::pair<double, double> velocity) const {
    CollisionData return_data;
    return_data.collision_time = 1.0; // Default to no collision this frame

    if (auto other_ent = other_entity.lock()) {
        auto this_trans = getTransform().lock();
        auto other_trans = other_ent->getTransform().lock();
        auto other_hbox = other_ent->getHitbox();

        if (this_trans && hbox_ && other_trans && other_hbox) {
            int x_entry_distance;
            int x_exit_distance;
            int y_exit_distance;
            int y_entry_distance;

            // Calculate distances between objects
            if (velocity.first > 0.0) {
                x_entry_distance = getAbsLeft(this_trans, hbox_) - getAbsRight(other_trans, other_hbox);
                x_exit_distance = getAbsRight(this_trans, hbox_) - getAbsLeft(other_trans, other_hbox);
            } else {
                x_entry_distance = getAbsRight(this_trans, hbox_) - getAbsLeft(other_trans, other_hbox);
                x_exit_distance = getAbsLeft(this_trans, hbox_) - getAbsRight(other_trans, other_hbox);
            }

            if (velocity.second > 0.0) {
                y_entry_distance = getAbsTop(this_trans, hbox_) - getAbsBottom(other_trans, other_hbox);
                y_exit_distance = getAbsBottom(this_trans, hbox_) - getAbsTop(other_trans, other_hbox);
            } else {
                y_entry_distance = getAbsBottom(this_trans, hbox_) - getAbsTop(other_trans, other_hbox);
                y_exit_distance = getAbsTop(this_trans, hbox_) - getAbsBottom(other_trans, other_hbox);
            }

            double x_entry_time;
            double x_exit_time;
            double y_entry_time;
            double y_exit_time;

            // Calculate time to impact/exit
            if (velocity.first == 0.0) {
                x_entry_time = -std::numeric_limits<double>::infinity();
                x_exit_time = std::numeric_limits<double>::infinity();
            } else {
                x_entry_time = x_entry_distance / velocity.first;
                x_exit_time = x_exit_distance / velocity.first;
            }

            if (velocity.second == 0.0) {
                y_entry_time = -std::numeric_limits<double>::infinity();
                y_exit_time = std::numeric_limits<double>::infinity();
            } else {
                y_entry_time = y_entry_distance / velocity.second;
                y_exit_time = y_exit_distance / velocity.second;
            }

            double latest_entry_time = std::max(x_entry_time, y_entry_time);
            double first_exit_time = std::min(x_exit_time, y_exit_time);

            if (latest_entry_time > first_exit_time ||
                    (x_entry_time < 0.0 && y_entry_time < 0.0) ||
                     x_entry_time > 1.0 || y_entry_time > 1.0) {
                return_data.normals = {0, 0};
                return_data.collision_time = 1.0;
            } else {
                if (x_entry_time > y_entry_time) {
                    if (x_entry_distance < 0) {
                        return_data.normals = {1, 0};
                    } else {
                        return_data.normals = {-1, 0};
                    }
                } else {
                    if (y_entry_distance < 0) {
                        return_data.normals = {0, 1};
                    } else {
                        return_data.normals = {0, -1};
                    }
                }
            }

            return_data.collision_time = latest_entry_time;
        }
    }

    return return_data;
}

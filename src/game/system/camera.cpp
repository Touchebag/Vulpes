#include "camera.h"

#include "system/world.h"

#include "utils/log.h"

#define FRACTION_OF_DISTANCE 0.15f

#define MAX_X_SPEED 30.0f
#define MAX_Y_SPEED 40.0f

#define MAX_X_ACCELERATION 5.0f
#define MAX_Y_ACCELERATION 5.0f

void Camera::setCameraBox(Camera::CameraBoundingBox camera_box) {
    camera_box_ = camera_box;
}

Camera::CameraBoundingBox Camera::getCameraBox() {
    return camera_box_;
}

void Camera::setView(float x, float y, float width, float height) {
    view_.x_pos = x;
    view_.y_pos = y;
    view_.width = width;
    view_.height = height;
}

void Camera::setView(CameraView view) {
    view_ = view;
}

Camera::CameraView Camera::getView() {
    return view_;
}

void Camera::setWindowSize(int width, int height) {
    aspect_ratio_ = static_cast<float>(width) / static_cast<float>(height);
}

void Camera::moveView(float x, float y) {
    auto width_offset = view_.width / 2.0f;
    auto height_offset = view_.height / 2.0f;

    if (x - width_offset < camera_box_.left_margin) {
        x = camera_box_.left_margin + width_offset;
    } else if (x + width_offset > camera_box_.right_margin) {
        x = camera_box_.right_margin - width_offset;
    }

    if (y - height_offset < camera_box_.top_margin) {
        y = camera_box_.top_margin + height_offset;
    } else if (y + height_offset > camera_box_.bottom_margin) {
        y = camera_box_.bottom_margin - height_offset;
    }

    view_.x_pos = x;
    view_.y_pos = y;
}

void Camera::resizeView(float width, float height) {
    view_.width = width;
    view_.height = height;
}

void Camera::update() {
    updateTargetView();

    // TODO Also add resize
    auto movement = calculateMovementToTarget();
    movement.x_pos = current_speed_.x_pos + std::max(std::min(movement.x_pos - current_speed_.x_pos, MAX_X_ACCELERATION), -MAX_X_ACCELERATION);
    movement.y_pos = current_speed_.y_pos + std::max(std::min(movement.y_pos - current_speed_.y_pos, MAX_Y_ACCELERATION), -MAX_Y_ACCELERATION);

    moveView(view_.x_pos + movement.x_pos, view_.y_pos + movement.y_pos);

    resizeView(target_view_.width, target_view_.height);
    current_speed_ = movement;
}

Camera::CameraView Camera::calculateMovementToTarget() {
    CameraView changes;

    // How much distance to close per frame
    // TODO If player is outside of screen snap immediately or increase speed
    changes.x_pos = std::min(std::max((target_view_.x_pos - view_.x_pos) * FRACTION_OF_DISTANCE, -MAX_X_SPEED), MAX_X_SPEED);
    changes.y_pos = std::min(std::max((target_view_.y_pos - view_.y_pos) * FRACTION_OF_DISTANCE, -MAX_Y_SPEED), MAX_Y_SPEED);

    return changes;
}

void Camera::updateTargetView() {
    // TODO Error handling
    auto player = World::getInstance<World::IWorldRead>().getPlayer().lock();

    if (player) {
        auto p_trans = player->trans_;
        auto p_move = player->movableEntity_;

        if (p_trans && p_move) {
            target_view_.x_pos = static_cast<float>(p_trans->getX() + (p_move->getVelX() * 20.0));
            target_view_.y_pos = static_cast<float>(p_trans->getY() + (p_move->getVelY() * 20.0));
        }
    }

    // Using height as base for nicer view
    target_view_.height = 1000.0f;
    target_view_.width = target_view_.height * aspect_ratio_;
}

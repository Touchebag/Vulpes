#include "camera.h"

#include "system/system.h"

#include "utils/log.h"
#include "utils/common.h"

#define FRACTION_OF_DISTANCE 0.15f
#define RESIZE_FRACTION 0.02f

#define X_RATIO_MULTIPLIER 600.0f
#define Y_RATIO_MULTIPLIER 600.0f

#define MAX_X_ACCELERATION 5.0f
#define MAX_Y_ACCELERATION 5.0f
#define MAX_WIDTH_ACCELERATION 0.5f
#define MAX_HEIGHT_ACCELERATION 0.5f

#define SHAKE_CONSTANT_X 20.0f
#define SHAKE_CONSTANT_Y 100.0f

namespace {

inline float floatRandom() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

} // namespace

void Camera::setCameraBox(Camera::CameraBoundingBox camera_box) {
    camera_box_ = camera_box;
}

Camera::CameraBoundingBox Camera::getCameraBox() {
    return camera_box_;
}

void Camera::setView(float x, float y, float width, float height) {
    CameraView view;

    view.x_pos = x;
    view.y_pos = y;
    view.width = width;
    view.height = height;

    setView(view);
}

void Camera::setView(CameraView view) {
    view_ = view;

    // Reset shaking camera
    adjusted_camera_view_ = view_;
}

Camera::CameraView Camera::getRawView() {
    return view_;
}

Camera::CameraView Camera::getView() {
    return adjusted_camera_view_;
}

Camera::CameraView Camera::getChangeVelocity() {
    return current_speed_;
}

void Camera::setWindowSize(int width, int height) {
    aspect_ratio_ = static_cast<float>(width) / static_cast<float>(height);

    auto view = getRawView();
    view.width = static_cast<float>(width);
    view.height = static_cast<float>(height);

    window_size_ = {width, height};

    setView(view);
}

std::pair<int, int> Camera::getWindowSize() {
    return window_size_;
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
    auto camera_box_width = camera_box_.right_margin - camera_box_.left_margin;
    auto camera_box_height = camera_box_.bottom_margin - camera_box_.top_margin;

    // Enforce aspect ratio
    width = height * aspect_ratio_;

    // Ensure neither direction spills outside camera box
    if (height > camera_box_height) {
        height = camera_box_height;
        width = height * aspect_ratio_;
    }

    if (width > camera_box_width) {
        width = camera_box_width;
        height = width / aspect_ratio_;
    }

    view_.width = width;
    view_.height = height;
}

void Camera::applyCameraShake() {
    adjusted_camera_view_ = view_;
    adjusted_camera_view_.x_pos += SHAKE_CONSTANT_X * floatRandom() * trauma_ * trauma_;
    adjusted_camera_view_.y_pos += SHAKE_CONSTANT_Y * floatRandom() * trauma_ * trauma_;

    addTrauma(-0.01f);
}

// Calculates the player position as a ratio of the current view
// This allows for consistent camera movement regardless of the current view/aspect ratio
std::pair<float, float> Camera::calculatePlayerPositionRatio(int x_pos, int y_pos) {
    // To prevent division by zero
    if (view_.width == 0.0 || view_.height == 0.0) {
        return {0.0, 0.0};
    }

    auto view_left = view_.x_pos - (view_.width / 2);
    auto view_top = view_.y_pos - (view_.height / 2);

    float player_x_ratio = (static_cast<float>(x_pos) - view_left) / view_.width;
    float player_y_ratio = (static_cast<float>(y_pos) - view_top) / view_.height;

    // Offset by 0.5 to get 0 in center
    return {player_x_ratio - 0.5f, player_y_ratio - 0.5f};
}

void Camera::addTrauma(float trauma) {
    trauma_ += trauma;

    trauma_ = std::max(std::min(trauma_, 1.0f), 0.0f);
}

void Camera::update() {
    updateTargetView();

    auto movement = calculateMovementToTarget();
    movement.x_pos = current_speed_.x_pos + std::max(std::min(movement.x_pos - current_speed_.x_pos, MAX_X_ACCELERATION), -MAX_X_ACCELERATION);
    movement.y_pos = current_speed_.y_pos + std::max(std::min(movement.y_pos - current_speed_.y_pos, MAX_Y_ACCELERATION), -MAX_Y_ACCELERATION);
    movement.width = current_speed_.width + std::max(std::min(movement.width - current_speed_.width, MAX_WIDTH_ACCELERATION), -MAX_WIDTH_ACCELERATION);
    movement.height = current_speed_.height + std::max(std::min(movement.height - current_speed_.height, MAX_HEIGHT_ACCELERATION), -MAX_HEIGHT_ACCELERATION);

    resizeView(view_.width + movement.width, view_.height + movement.height);
    moveView(view_.x_pos + movement.x_pos, view_.y_pos + movement.y_pos);

    applyCameraShake();

    current_speed_ = movement;
}

Camera::CameraView Camera::calculateMovementToTarget() {
    CameraView changes;

    // How much distance to close per frame
    changes.x_pos  = (target_view_.x_pos  - view_.x_pos)  * FRACTION_OF_DISTANCE;
    changes.y_pos  = (target_view_.y_pos  - view_.y_pos)  * FRACTION_OF_DISTANCE;
    changes.width  = (target_view_.width  - view_.width)  * RESIZE_FRACTION;
    changes.height = (target_view_.height - view_.height) * RESIZE_FRACTION;

    return changes;
}

void Camera::updateTargetView() {
    // TODO Error handling
    auto player = System::IWorldRead::getPlayer().lock();

    if (player) {
        auto p_trans = player->getComponent<Transform>();
        auto p_move = player->getComponent<Movement>();

        if (p_trans && p_move) {
            auto ratio = calculatePlayerPositionRatio(p_trans->getX(), p_trans->getY());
            target_view_.x_pos = view_.x_pos + ratio.first * X_RATIO_MULTIPLIER;
            target_view_.y_pos = view_.y_pos + ratio.second * Y_RATIO_MULTIPLIER;

            // Check if player is moving
            if (!(util::closeToZero(p_move->getVelX(), 0.01) && util::closeToZero(p_move->getVelY(), 0.01))) {
                idle_frame_counter_ = 210;
            } else {
                idle_frame_counter_--;
            }
        }
    }

    // Zoom in when standing still
    if (idle_frame_counter_ <= 0) {
        // Using height as base for nicer view
        target_view_.height = 1000.0f;
    } else {
        // Using height as base for nicer view
        target_view_.height = 2000.0f;
    }

    auto half_width = view_.width / 2.0f;
    auto half_height = view_.height / 2.0f;

    if (target_view_.x_pos < camera_box_.left_margin + half_width) {
        target_view_.x_pos = camera_box_.left_margin + half_width;
    } else if (target_view_.x_pos > camera_box_.right_margin - half_width) {
        target_view_.x_pos = camera_box_.right_margin - half_width;
    }

    if (target_view_.y_pos < camera_box_.top_margin + half_height) {
        target_view_.y_pos = camera_box_.top_margin + half_height;
    } else if (target_view_.y_pos > camera_box_.bottom_margin - half_height) {
        target_view_.y_pos = camera_box_.bottom_margin - half_height;
    }
}

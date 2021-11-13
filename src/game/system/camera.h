#pragma once

#include <SFML/Graphics.hpp>

#include "utils/common.h"

class Camera {
  public:
    struct CameraBoundingBox {
        float left_margin;
        float right_margin;
        float top_margin;
        float bottom_margin;
    };

    struct CameraView {
        float x_pos;
        float y_pos;
        float width;
        float height;
    };

    void update();

    void setView(float x, float y, float width, float height);
    void setView(CameraView view);
    CameraView getView();
    CameraView getRawView();

    CameraView getChangeVelocity();

    void setWindowSize(int width, int height);
    std::pair<int, int> getWindowSize();

    void setCameraBox(CameraBoundingBox camera_box);

    void addTrauma(float trauma);

    CameraBoundingBox getCameraBox();

    // Returns the position relative to camera space from 0,0 (upper left) to 1,1 (lower right)
    // Can go outside range if position is outside camera
    std::pair<double, double> getRelativePosition(double x, double y);

  private:
    void moveView(float x, float y);
    void resizeView(float width, float height);

    void updateTargetView();

    float aspect_ratio_ = 1.0f;

    void applyCameraShake();

    std::pair<int, int> window_size_;

    CameraView calculateMovementToTarget();

    CameraView view_;
    CameraBoundingBox camera_box_;

    // This is the final view with applied effects such as shake
    // Saved as a different variable to allow updating at a different interval
    // than rendering
    CameraView adjusted_camera_view_;

    CameraView target_view_;

    // Used for acceleration smoothing
    CameraView current_speed_;

    // Used to delay view shrinking when idle
    int idle_frame_counter_ = 0;

    float trauma_ = 0.0f;
};

#pragma once

#include <SFML/Graphics.hpp>

#include "utils/common.h"

class Camera {
  public:
    struct CameraBoundingBox {
        double left_margin;
        double right_margin;
        double top_margin;
        double bottom_margin;
    };

    struct CameraView {
        double x_pos;
        double y_pos;
        double width;
        double height;
    };

    void update();

    void setView(double x, double y, double width, double height);
    void setView(const CameraView& view);
    CameraView getView();
    CameraView getRawView();

    CameraView getChangeVelocity();

    void setWindowSize(unsigned int width, unsigned int height);
    std::pair<unsigned int, unsigned int> getWindowSize();

    void setCameraBox(const CameraBoundingBox& camera_box);

    void addTrauma(double trauma);

    CameraBoundingBox getCameraBox();

    // Returns the position relative to camera space from 0,0 (upper left) to 1,1 (lower right)
    // Can go outside range if position is outside camera
    std::pair<double, double> getRelativePosition(double x, double y);

  private:
    void moveView(double x, double y);
    void resizeView(double width, double height);

    void updateTargetView();

    double aspect_ratio_ = 1.0;

    void applyCameraShake();

    std::pair<int, int> window_size_;

    CameraView calculateMovementToTarget();

    CameraView view_ = {};
    CameraBoundingBox camera_box_ = {};

    // This is the final view with applied effects such as shake
    // Saved as a different variable to allow updating at a different interval
    // than rendering
    CameraView adjusted_camera_view_ = {};

    CameraView target_view_ = {};

    // Used for acceleration smoothing
    CameraView current_speed_ = {};

    // Used to delay view shrinking when idle
    int idle_frame_counter_ = 0;

    double trauma_ = 0.0;
};

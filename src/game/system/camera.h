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

    void setWindowSize(int width, int height);

    void setCameraBox(CameraBoundingBox camera_box);

    CameraBoundingBox getCameraBox();

  private:
    void moveView(float x, float y);
    void resizeView(float width, float height);

    void updateTargetView();

    float aspect_ratio_ = 1.0f;

    CameraView calculateMovementToTarget();

    CameraView view_;
    CameraBoundingBox camera_box_;

    CameraView target_view_;

    // Used for acceleration smoothing
    CameraView current_speed_;
};

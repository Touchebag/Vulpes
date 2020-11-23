#pragma once

#include <SFML/Graphics.hpp>

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

    void moveView(float x, float y);
    void resizeView(float width, float height);
    void setView(float x, float y, float width, float height);
    CameraView getView();

    void setCameraBox(CameraBoundingBox camera_box);

    CameraBoundingBox getCameraBox();

  private:
    CameraView view_;
    CameraBoundingBox camera_box_;
};

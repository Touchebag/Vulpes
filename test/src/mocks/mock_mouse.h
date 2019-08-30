#pragma once

class MockMouse {
  public:
    static std::pair<int, int> getMousePosition();
    static std::pair<float, float> getMouseWorldPosition();

    static void setMousePosition(std::pair<int, int> position);
    static void setMouseWorldPosition(std::pair<float, float> position);
  private:
    static std::pair<int, int> mock_mouse_pos_;
    static std::pair<float, float> mock_mouse_world_pos_;
};

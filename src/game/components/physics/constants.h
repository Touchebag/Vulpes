#pragma once

#include <limits>

struct PhysicsConstants {
    // Horizontal movement
    double x_acceleration = 0.0;
    double x_friction = 0.0;

    // Vertical movement
    double y_acceleration = 0.0;
    double y_friction = 0.0;
    double gravity = 0.0;
    double jump_multiplier = 1.0;

    // Other
    double jump_impulse_x = std::numeric_limits<double>::signaling_NaN();
    double jump_impulse_y = std::numeric_limits<double>::signaling_NaN();
    double dash_speed = 0.0;
    double dash_friction = 0.0;
};


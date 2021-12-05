#pragma once

#include <limits>

struct PhysicsConstants {
    // Horizontal movement
    double x_acceleration = 0.0;
    double x_friction = 0.0;

    // Vertical movement
    double gravity = 0.0;
    double fall_multiplier = 1.0;
    double low_jump_multiplier = 1.0;
    double max_vertical_speed = std::numeric_limits<double>::max();
    double min_vertical_speed = -std::numeric_limits<double>::max();
    double max_air_dive_speed = std::numeric_limits<double>::max();
    double air_dive_multiplier = 1.0;

    // Other
    double jump_impulse = 0.0;
    double wall_slide_friction = 0.0;
    double wall_jump_horizontal_impulse = 0.0;
    double wall_jump_vertical_impulse = 0.0;
    double dash_speed = 0.0;
    double dash_friction = 0.0;
    double air_dive_impulse = 0.0;
};


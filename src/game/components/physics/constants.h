#pragma once

struct PhysicsConstants {
    // Ground movement
    double ground_acceleration = 0.0;
    double ground_friction = 0.0;

    // Air movement
    double air_acceleration = 0.0;
    double air_friction = 0.0;
    double gravity = 0.0;
    double fall_multiplier = 0.0;
    double low_jump_multiplier = 0.0;
    double max_vertical_speed = 0.0;
    double min_vertical_speed = 0.0;
    double max_air_dive_speed = 0.0;
    double air_dive_multiplier = 0.0;

    // Other
    double jump_impulse = 0.0;
    double wall_slide_friction = 0.0;
    double wall_jump_horizontal_impulse = 0.0;
    double wall_jump_vertical_impulse = 0.0;
    double dash_speed = 0.0;
    double dash_friction = 0.0;
    double air_dive_impulse = 0.0;
};


#include <gtest/gtest.h>

#include "base_entity.h"

nlohmann::json entity_json = nlohmann::json::parse(R"--(
{
    "Actions": null,
    "Animated": {
        "frame_list": [
            "Idle0",
            "Idle9"
        ],
        "sprite_map": "Player.txt"
    },
    "AI": {
        "file": "basic_enemy.json"
    },
    "Collision": {
        "type": "player_hurtbox"
    },
    "Damageable": {
        "health": 100
    },
    "Death": null,
    "Hitbox": {
        "height": 200,
        "width": 50
    },
    "Movable": null,
    "Physics": {
        "ground_acceleration": 7.0,
        "ground_friction": 0.7,
        "air_acceleration": 2.0,
        "air_friction": 0.93,
        "gravity": 1.0,
        "fall_multiplier": 3.5,
        "low_jump_multiplier": 2.5,
        "max_vertical_speed": 20.0,
        "min_vertical_speed": -80.0,
        "jump_impulse": -30.0,
        "wall_slide_friction": 0.85,
        "wall_jump_horizontal_impulse": 30.0,
        "wall_jump_vertical_impulse": -25.0,
        "dash_speed": 100.0,
        "dash_friction": 0.8
    },
    "Renderable": {
        "scale": 0.4000000059604645,
        "texture": "Player.png",
        "tile_x": true,
        "tile_y": true
    },
    "Stateful": {
        "states": "player_state.json"
    },
    "Subentity": null,
    "Transform": {
        "pos_x": 350,
        "pos_y": 1300
    }
}
)--");

TEST(TestComponents, TestLoadSaveAllComponents) {
    std::shared_ptr<BaseEntity> entity = BaseEntity::createFromJson(entity_json);

    auto e1 = entity->outputToJson();
    ASSERT_TRUE(e1.has_value());

    nlohmann::json j1 = e1.value();

    EXPECT_TRUE(entity_json == j1);

    entity->reloadFromJson(j1);

    auto e2 = entity->outputToJson();
    ASSERT_TRUE(e2.has_value());

    nlohmann::json j2 = e2.value();

    EXPECT_TRUE(j1 == j2);
}

#include <gtest/gtest.h>

#include <iostream>

#include "system/world.h"
#include "utils/file.h"

class WorldTestFixture : public ::testing::Test {
  public:
    WorldTestFixture() {
    }

    void SetUp() {
    }
};

TEST_F(WorldTestFixture, SaveLoadWorld) {
    nlohmann::json world_json = File::loadRoom("test_world_2.json").value();

    World::getInstance().loadWorldFromJson(world_json);
    nlohmann::json world_json_2 = World::getInstance().saveWorldToJson();

    // _EQ does not work with nlohmann::json
    ASSERT_TRUE(world_json == world_json_2);
}

TEST_F(WorldTestFixture, SaveLoadWorldFailure) {
    nlohmann::json world_json = File::loadRoom("test_world_2.json").value();

    World::getInstance().loadWorldFromJson(world_json);

    nlohmann::json j = nlohmann::json::parse( R"--(
        {
            "Hitbox": {
                "height": 200,
                "width": 50
            },
            "Renderable": {
                "scale": 1.0,
                "texture": "box.png"
            },
            "Transform": {
                "pos_x": 200,
                "pos_y": 100
            }
        })--");

    auto hud_object = std::make_shared<BaseEntity>();
    hud_object->loadFromJson(j);

    World::getInstance().addEntity(hud_object, World::Layer::MAIN);

    nlohmann::json world_json_2 = World::getInstance().saveWorldToJson();

    // _EQ does not work with nlohmann::json
    ASSERT_FALSE(world_json == world_json_2);
}

TEST_F(WorldTestFixture, EnsureHudNotSaved) {
    nlohmann::json world_json = File::loadRoom("test_world_2.json").value();

    World::getInstance().loadWorldFromJson(world_json);

    nlohmann::json j = nlohmann::json::parse( R"--(
        {
            "Hitbox": {
                "height": 200,
                "width": 50
            },
            "Renderable": {
                "scale": 1.0,
                "texture": "box.png"
            },
            "Transform": {
                "pos_x": 200,
                "pos_y": 100
            }
        })--");

    auto hud_object = std::make_shared<BaseEntity>();
    hud_object->loadFromJson(j);

    World::getInstance().addEntity(hud_object, World::Layer::HUD);

    nlohmann::json world_json_2 = World::getInstance().saveWorldToJson();

    // _EQ does not work with nlohmann::json
    ASSERT_TRUE(world_json == world_json_2);
}

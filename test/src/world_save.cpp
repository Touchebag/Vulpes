#include <gtest/gtest.h>

#include <iostream>

#include "system/system.h"
#include "utils/file.h"

class WorldTestFixture : public ::testing::Test {
  public:
    WorldTestFixture() {
    }

    void SetUp() override {
    }
};

TEST_F(WorldTestFixture, SaveLoadWorld) {
    nlohmann::json world_json = File().loadRoom("test_world_2.json").value();

    System::IWorldModify::loadWorldFromJson(world_json);
    nlohmann::json world_json_2 = System::IWorldModify::saveWorldToJson();

    // _EQ does not work with nlohmann::json
    ASSERT_TRUE(world_json == world_json_2) << world_json.dump() << std::endl << world_json_2.dump() << std::endl;
}

TEST_F(WorldTestFixture, SaveLoadWorldFailure) {
    nlohmann::json world_json = File().loadRoom("test_world_2.json").value();

    System::IWorldModify::loadWorldFromJson(world_json);

    nlohmann::json j = nlohmann::json::parse( R"--(
        {
            "Hitbox": {
                "height": 200,
                "width": 50
            },
            "Rendering": {
                "scale": 1.0,
                "texture": "box.png",
                "layer": 0
            },
            "Transform": {
                "pos_x": 200,
                "pos_y": 100
            }
        })--");

    auto hud_object = BaseEntity::createFromJson(j);

    System::IWorldModify::addEntity(hud_object);

    nlohmann::json world_json_2 = System::IWorldModify::saveWorldToJson();

    // _EQ does not work with nlohmann::json
    ASSERT_FALSE(world_json == world_json_2) << world_json.dump() << std::endl << world_json_2.dump() << std::endl;
}

TEST_F(WorldTestFixture, EnsureHudNotSaved) {
    nlohmann::json world_json = File().loadRoom("test_world_2.json").value();

    System::IWorldModify::loadWorldFromJson(world_json);

    nlohmann::json j = nlohmann::json::parse( R"--(
        {
            "Hitbox": {
                "height": 200,
                "width": 50
            },
            "Rendering": {
                "scale": 1.0,
                "texture": "box.png",
                "layer": 0
            },
            "Transform": {
                "pos_x": 200,
                "pos_y": 100
            }
        })--");

    auto hud_object = BaseEntity::createFromJson(j);
    hud_object->getComponent<Rendering>()->setLayer(INT_MAX);

    System::IWorldModify::addEntity(hud_object);

    nlohmann::json world_json_2 = System::IWorldModify::saveWorldToJson();

    // _EQ does not work with nlohmann::json
    ASSERT_TRUE(world_json == world_json_2) << world_json.dump() << std::endl << world_json_2.dump() << std::endl;
}

TEST_F(WorldTestFixture, TestLoadConditionalEntity) {
    using w_modify = System::IWorldModify;

    w_modify::clearWorld();

    ASSERT_TRUE(w_modify::getWorldObjects().empty());

    nlohmann::json j = nlohmann::json::parse( R"--(
        {
            "entities": [
                {
                    "condition": "test_conditional_entity_flag"
                }
            ]
        })--");

    // Should not be added since flag is not set
    w_modify::loadWorldFromJson(j);
    EXPECT_TRUE(w_modify::getWorldObjects().empty());

    // After setting flag entity should be added
    System::getEnvironment()->setFlag("test_conditional_entity_flag");
    w_modify::loadWorldFromJson(j);
    EXPECT_TRUE(w_modify::getWorldObjects().size() == 1);
}

TEST_F(WorldTestFixture, TestLoadConditionalEntityNegated) {
    using w_modify = System::IWorldModify;

    w_modify::clearWorld();

    ASSERT_TRUE(w_modify::getWorldObjects().empty());

    nlohmann::json j = nlohmann::json::parse( R"--(
        {
            "entities": [
                {
                    "condition": "!test_conditional_entity_flag_negated"
                }
            ]
        })--");

    // Should be added since flag is not set
    w_modify::loadWorldFromJson(j);
    EXPECT_TRUE(w_modify::getWorldObjects().size() == 1);

    // After setting flag entity should not be added
    System::getEnvironment()->setFlag("test_conditional_entity_flag_negated");
    w_modify::loadWorldFromJson(j);
    EXPECT_TRUE(w_modify::getWorldObjects().empty());
}

TEST_F(WorldTestFixture, TestSaveLoadTemplate) {
    nlohmann::json world_json = File().loadRoom("test_world_template.json").value();

    System::IWorldModify::loadWorldFromJson(world_json);
    nlohmann::json world_json_2 = System::IWorldModify::saveWorldToJson();

    // _EQ does not work with nlohmann::json
    ASSERT_TRUE(world_json == world_json_2) << world_json.dump() << std::endl << world_json_2.dump() << std::endl;
}

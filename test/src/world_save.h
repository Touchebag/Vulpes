#include <gtest/gtest.h>

#include <iostream>

#include "world.h"
#include "file.h"

class WorldTestFixture : public ::testing::Test {
  public:
    WorldTestFixture() {
    }

    void SetUp() {
    }
};

TEST_F(WorldTestFixture, SaveLoadWorld) {
    nlohmann::json world_json = file::loadJson("test_data/test_world_2.json").value();

    World::getInstance().loadWorldFromJson(world_json);
    nlohmann::json world_json_2 = World::getInstance().saveWorldToJson();

    // _EQ does not work with nlohmann::json
    ASSERT_TRUE(world_json == world_json_2);
}

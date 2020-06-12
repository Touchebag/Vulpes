#include <gtest/gtest.h>

#include "system/world.h"

#include "components/transform.h"

class DynamicCollisionTestFixture : public ::testing::Test {
  public:
    DynamicCollisionTestFixture() {
        entity_ = BaseEntity::createFromJson(nlohmann::json::parse(entity_json_));
    }

    void SetUp() override {
        World::getInstance<World::IWorldModify>().loadWorldFromFile("test_world.json");
    }

    std::string entity_json_ = R"--(
{
    "Transform": {
        "pos_x": 100,
        "pos_y": 100
    },
    "Movable": null,
    "Collision": {
        "type": "static",
        "height": 50,
        "width": 50
    }
}
        )--";

    std::shared_ptr<BaseEntity> entity_;
};

TEST_F(DynamicCollisionTestFixture, MoveSingleDirectionNoCollision) {
    World::getInstance<World::IWorldModify>().addEntity(entity_);

    auto max_mvmnt = entity_->movableEntity_->getMaximumMovement(10, 0);

    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    auto pos_x = entity_->trans_->getX();
    auto pos_y = entity_->trans_->getY();

    EXPECT_EQ(110, pos_x);
    EXPECT_EQ(100, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveSingleDirectionCollision) {
    World::getInstance<World::IWorldModify>().addEntity(entity_);

    auto max_mvmnt = entity_->movableEntity_->getMaximumMovement(60, 0);
    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    auto pos_x = entity_->trans_->getX();
    auto pos_y = entity_->trans_->getY();

    EXPECT_EQ(150, pos_x);
    EXPECT_EQ(100, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveSingleDirectionNegativeCollision) {
    World::getInstance<World::IWorldModify>().addEntity(entity_);

    auto max_mvmnt = entity_->movableEntity_->getMaximumMovement(0, -70);
    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    auto pos_x = entity_->trans_->getX();
    auto pos_y = entity_->trans_->getY();

    EXPECT_EQ(100, pos_x);
    EXPECT_EQ(50, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveMultipleDirectionCollisionInOne) {
    World::getInstance<World::IWorldModify>().addEntity(entity_);

    auto max_mvmnt = entity_->movableEntity_->getMaximumMovement(80, 20);
    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    auto pos_x = entity_->trans_->getX();
    auto pos_y = entity_->trans_->getY();

    EXPECT_EQ(150, pos_x);
    EXPECT_EQ(120, pos_y);
}

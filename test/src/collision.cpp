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

TEST_F(DynamicCollisionTestFixture, CollisionSemiSolidAllDirections) {
    World::getInstance<World::IWorldModify>().addEntity(entity_);

    // From below, should not be stopped
    entity_->trans_->setPosition(500, 200);

    auto max_mvmnt = entity_->movableEntity_->getMaximumMovement(0, -100);
    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    auto pos_x = entity_->trans_->getX();
    auto pos_y = entity_->trans_->getY();

    EXPECT_EQ(500, pos_x);
    EXPECT_EQ(100, pos_y);

    // From above, should be stopped
    entity_->trans_->setPosition(500, 0);

    max_mvmnt = entity_->movableEntity_->getMaximumMovement(0, 100);
    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    pos_x = entity_->trans_->getX();
    pos_y = entity_->trans_->getY();

    EXPECT_EQ(500, pos_x);
    EXPECT_EQ(50, pos_y);

    // Left/right, should not be stopped
    entity_->trans_->setPosition(400, 100);

    max_mvmnt = entity_->movableEntity_->getMaximumMovement(200, 0);
    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    pos_x = entity_->trans_->getX();
    pos_y = entity_->trans_->getY();

    EXPECT_EQ(600, pos_x);
    EXPECT_EQ(100, pos_y);

    max_mvmnt = entity_->movableEntity_->getMaximumMovement(-200, 0);
    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    pos_x = entity_->trans_->getX();
    pos_y = entity_->trans_->getY();

    EXPECT_EQ(400, pos_x);
    EXPECT_EQ(100, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveDiagonalStuckOnCorner) {
    World::getInstance<World::IWorldModify>().clearWorld();

    std::string entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 20,
        "width": 20
    },
    "Transform": {
        "pos_x": 20,
        "pos_y": 20
    }
}
    )--";

    auto entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    World::IWorldModify::addEntity(entity);

    entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 10,
        "width": 10
    },
    "Movable": null,
    "Transform": {
        "pos_x": 0,
        "pos_y": 0
    }
}
    )--";

    entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    World::IWorldModify::addEntity(entity);

    auto max_mvmnt = entity->movableEntity_->getMaximumMovement(10, 10);
    entity->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    auto pos_x = entity->trans_->getX();
    auto pos_y = entity->trans_->getY();

    // Should continue moving on top of object instead of getting stuck in corner
    EXPECT_EQ(10, pos_x);
    EXPECT_EQ(5, pos_y);
}

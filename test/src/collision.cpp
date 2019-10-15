#include <gtest/gtest.h>

#include "world.h"

#include "components/hitbox.h"
#include "components/transform.h"

class DynamicCollisionTestFixture : public ::testing::Test {
  public:
    DynamicCollisionTestFixture() {
        entity_ = std::make_shared<BaseEntity>();
        entity_->loadFromJson(nlohmann::json::parse(entity_json_));
    }

    void SetUp() {
        World::getInstance().loadWorldFromFile("test_data/test_world.json");
    }

    std::string entity_json_ = R"--(
{
    "Hitbox": {
        "height": 50,
        "width": 50
    },
    "Transform": {
        "pos_x": 100,
        "pos_y": 100
    },
    "Movable": null,
    "Collision": null
}
        )--";

    std::shared_ptr<BaseEntity> entity_;
};

TEST_F(DynamicCollisionTestFixture, MoveSingleDirectionNoCollision) {
    World::getInstance().addEntity(entity_, World::Layer::MAIN);

    auto max_mvmnt = entity_->movableEntity_->getMaximumMovement(10, 0);

    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    auto pos_x = entity_->trans_->getX();
    auto pos_y = entity_->trans_->getY();

    EXPECT_EQ(110, pos_x);
    EXPECT_EQ(100, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveSingleDirectionCollision) {
    World::getInstance().addEntity(entity_, World::Layer::MAIN);

    auto max_mvmnt = entity_->movableEntity_->getMaximumMovement(60, 0);
    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    auto pos_x = entity_->trans_->getX();
    auto pos_y = entity_->trans_->getY();

    EXPECT_EQ(150, pos_x);
    EXPECT_EQ(100, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveSingleDirectionNegativeCollision) {
    World::getInstance().addEntity(entity_, World::Layer::MAIN);

    auto max_mvmnt = entity_->movableEntity_->getMaximumMovement(0, -70);
    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    auto pos_x = entity_->trans_->getX();
    auto pos_y = entity_->trans_->getY();

    EXPECT_EQ(100, pos_x);
    EXPECT_EQ(50, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveMultipleDirectionCollisionInOne) {
    World::getInstance().addEntity(entity_, World::Layer::MAIN);

    auto max_mvmnt = entity_->movableEntity_->getMaximumMovement(80, 20);
    entity_->movableEntity_->move(max_mvmnt.first, max_mvmnt.second);
    auto pos_x = entity_->trans_->getX();
    auto pos_y = entity_->trans_->getY();

    EXPECT_EQ(150, pos_x);
    EXPECT_EQ(120, pos_y);
}


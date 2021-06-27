#include <gtest/gtest.h>

#include "system/system.h"

#include "components/transform.h"

class DynamicCollisionTestFixture : public ::testing::Test {
  public:
    DynamicCollisionTestFixture() {
        entity_ = BaseEntity::createFromJson(nlohmann::json::parse(entity_json_));
        System::setCamera({});
    }

    ~DynamicCollisionTestFixture() {
        System::setCamera(std::make_shared<Camera>());
    }

    void SetUp() override {
        System::IWorldModify().clearWorld();
        System::IWorldModify::loadWorldFromFile("test_world.json");
    }

    std::string entity_json_ = R"--(
{
    "Transform": {
        "pos_x": 100,
        "pos_y": 100
    },
    "Movement": null,
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
    System::IWorldModify::addEntity(entity_);

    entity_->getComponent<Movement>()->move(10, 0);
    auto pos_x = entity_->getComponent<Transform>()->getX();
    auto pos_y = entity_->getComponent<Transform>()->getY();

    EXPECT_EQ(110, pos_x);
    EXPECT_EQ(100, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveSingleDirectionCollision) {
    System::IWorldModify::addEntity(entity_);

    entity_->getComponent<Movement>()->move(60, 0);
    auto pos_x = entity_->getComponent<Transform>()->getX();
    auto pos_y = entity_->getComponent<Transform>()->getY();

    EXPECT_EQ(150, pos_x);
    EXPECT_EQ(100, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveSingleDirectionNegativeCollision) {
    System::IWorldModify::addEntity(entity_);

    entity_->getComponent<Movement>()->move(0, -70);
    auto pos_x = entity_->getComponent<Transform>()->getX();
    auto pos_y = entity_->getComponent<Transform>()->getY();

    EXPECT_EQ(100, pos_x);
    EXPECT_EQ(50, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveMultipleDirectionCollisionInOne) {
    System::IWorldModify::addEntity(entity_);

    entity_->getComponent<Movement>()->move(80, 20);
    auto pos_x = entity_->getComponent<Transform>()->getX();
    auto pos_y = entity_->getComponent<Transform>()->getY();

    EXPECT_EQ(150, pos_x);
    EXPECT_EQ(120, pos_y);
}

TEST_F(DynamicCollisionTestFixture, CollisionSemiSolidAllDirections) {
    System::IWorldModify::addEntity(entity_);

    // From below, should not be stopped
    entity_->getComponent<Transform>()->setPosition(500, 200);

    entity_->getComponent<Movement>()->move(0, -100);
    auto pos_x = entity_->getComponent<Transform>()->getX();
    auto pos_y = entity_->getComponent<Transform>()->getY();

    EXPECT_EQ(500, pos_x);
    EXPECT_EQ(100, pos_y);

    // From above, should be stopped
    entity_->getComponent<Transform>()->setPosition(500, 0);

    entity_->getComponent<Movement>()->move(0, 100);
    pos_x = entity_->getComponent<Transform>()->getX();
    pos_y = entity_->getComponent<Transform>()->getY();

    EXPECT_EQ(500, pos_x);
    EXPECT_EQ(50, pos_y);

    // Left/right, should not be stopped
    entity_->getComponent<Transform>()->setPosition(400, 100);

    entity_->getComponent<Movement>()->move(200, 0);
    pos_x = entity_->getComponent<Transform>()->getX();
    pos_y = entity_->getComponent<Transform>()->getY();

    EXPECT_EQ(600, pos_x);
    EXPECT_EQ(100, pos_y);

    entity_->getComponent<Movement>()->move(-200, 0);
    pos_x = entity_->getComponent<Transform>()->getX();
    pos_y = entity_->getComponent<Transform>()->getY();

    EXPECT_EQ(400, pos_x);
    EXPECT_EQ(100, pos_y);
}

TEST_F(DynamicCollisionTestFixture, MoveDiagonalStuckOnCorner) {
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
    System::IWorldModify::addEntity(entity);

    entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 10,
        "width": 10
    },
    "Movement": null,
    "Transform": {
        "pos_x": 0,
        "pos_y": 0
    }
}
    )--";

    entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    System::IWorldModify::addEntity(entity);

    entity->getComponent<Movement>()->move(10, 10);
    auto pos_x = entity->getComponent<Transform>()->getX();
    auto pos_y = entity->getComponent<Transform>()->getY();

    // Should continue moving on top of object instead of getting stuck in corner
    EXPECT_EQ(10, pos_x);
    EXPECT_EQ(5, pos_y);
}

TEST_F(DynamicCollisionTestFixture, TunnelingOneDirection) {
    std::string entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 20,
        "width": 10
    },
    "Transform": {
        "pos_x": 15,
        "pos_y": 5
    }
}
    )--";

    auto entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    System::IWorldModify::addEntity(entity);

    entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 10,
        "width": 10
    },
    "Movement": null,
    "Transform": {
        "pos_x": 0,
        "pos_y": 0
    }
}
    )--";

    entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    System::IWorldModify::addEntity(entity);

    entity->getComponent<Movement>()->move(30, 0);
    auto pos_x = entity->getComponent<Transform>()->getX();
    auto pos_y = entity->getComponent<Transform>()->getY();

    // Should not tunnel through but get stuck in corner
    EXPECT_EQ(5, pos_x);
    EXPECT_EQ(0, pos_y);
}

TEST_F(DynamicCollisionTestFixture, TunnelingStuckInCorner) {
    std::string entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 20,
        "width": 10
    },
    "Transform": {
        "pos_x": 15,
        "pos_y": 5
    }
}
    )--";

    auto entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    System::IWorldModify::addEntity(entity);

    entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 10,
        "width": 20
    },
    "Transform": {
        "pos_x": 0,
        "pos_y": 20
    }
}
    )--";

    entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    System::IWorldModify::addEntity(entity);

    entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 10,
        "width": 10
    },
    "Movement": null,
    "Transform": {
        "pos_x": 0,
        "pos_y": 0
    }
}
    )--";

    entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    System::IWorldModify::addEntity(entity);

    entity->getComponent<Movement>()->move(30, 35);
    auto pos_x = entity->getComponent<Transform>()->getX();
    auto pos_y = entity->getComponent<Transform>()->getY();

    // Should not tunnel through but get stuck in corner
    EXPECT_EQ(5, pos_x);
    EXPECT_EQ(10, pos_y);
}

TEST_F(DynamicCollisionTestFixture, TunnelingOnlyXSlideDown) {
    std::string entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 20,
        "width": 10
    },
    "Transform": {
        "pos_x": 15,
        "pos_y": 5
    }
}
    )--";

    auto entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    System::IWorldModify::addEntity(entity);

    entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 10,
        "width": 10
    },
    "Movement": null,
    "Transform": {
        "pos_x": 0,
        "pos_y": 0
    }
}
    )--";

    entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    System::IWorldModify::addEntity(entity);

    entity->getComponent<Movement>()->move(30, 35);
    auto pos_x = entity->getComponent<Transform>()->getX();
    auto pos_y = entity->getComponent<Transform>()->getY();

    // Should not tunnel through X but get slide in Y direction
    EXPECT_EQ(5, pos_x);
    EXPECT_EQ(35, pos_y);
}

TEST_F(DynamicCollisionTestFixture, GrazeAgainstCornerButNoCollision) {
    std::string entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 10,
        "width": 10
    },
    "Transform": {
        "pos_x": 31,
        "pos_y": 9
    }
}
    )--";

    auto entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    System::IWorldModify::addEntity(entity);

    entity_json = R"--(
{
    "Collision": {
        "type": "static",
        "height": 10,
        "width": 10
    },
    "Movement": null,
    "Transform": {
        "pos_x": 0,
        "pos_y": 0
    }
}
    )--";

    entity = BaseEntity::createFromJson(nlohmann::json::parse(entity_json));
    System::IWorldModify::addEntity(entity);

    entity->getComponent<Movement>()->move(40, 40);
    auto pos_x = entity->getComponent<Transform>()->getX();
    auto pos_y = entity->getComponent<Transform>()->getY();

    // Should touch corner of box but not be blocked
    EXPECT_EQ(40, pos_x);
    EXPECT_EQ(40, pos_y);
}

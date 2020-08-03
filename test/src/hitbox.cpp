#include <gtest/gtest.h>

#include "components/collision/movement/collision_static.h"
#include "components/transform.h"

class StaticCollisionTestFixture : public ::testing::Test {
  public:
    void SetUp() override {
        trans1_ = std::make_shared<Transform>();
        trans2_ = std::make_shared<Transform>();
    }

    std::shared_ptr<Transform> trans1_;
    std::shared_ptr<Transform> trans2_;
};

TEST_F(StaticCollisionTestFixture, CollideAtOrigin) {
    std::shared_ptr<CollisionStatic> c1 = std::make_shared<CollisionStatic>(trans1_);
    std::shared_ptr<CollisionStatic> c2 = std::make_shared<CollisionStatic>(trans2_);

    c1->setHitbox(10, 20);
    c2->setHitbox(5, 30);

    EXPECT_TRUE(c1->collides(c2));
}

TEST_F(StaticCollisionTestFixture, CollideAtOffset) {
    std::shared_ptr<CollisionStatic> c1 = std::make_shared<CollisionStatic>(trans1_);
    std::shared_ptr<CollisionStatic> c2 = std::make_shared<CollisionStatic>(trans2_);

    c1->setHitbox(10, 20);
    trans1_->setPosition(5, 10);
    c2->setHitbox(20, 30);
    trans2_->setPosition(5, 20);

    EXPECT_TRUE(c1->collides(c2));
}

TEST_F(StaticCollisionTestFixture, NotColliding) {
    std::shared_ptr<CollisionStatic> c1 = std::make_shared<CollisionStatic>(trans1_);
    std::shared_ptr<CollisionStatic> c2 = std::make_shared<CollisionStatic>(trans2_);

    c1->setHitbox(10, 10);
    trans1_->setPosition(20, 50);
    c2->setHitbox(10, 10);
    trans2_->setPosition(-40, 10);

    EXPECT_FALSE(c1->collides(c2));
}

TEST_F(StaticCollisionTestFixture, NotCollidingTouching) {
    std::shared_ptr<CollisionStatic> c1 = std::make_shared<CollisionStatic>(trans1_);
    std::shared_ptr<CollisionStatic> c2 = std::make_shared<CollisionStatic>(trans2_);

    c1->setHitbox(10, 10);
    trans1_->setPosition(5, 5);
    c2->setHitbox(10, 10);
    trans2_->setPosition(-5, -5);

    EXPECT_FALSE(c1->collides(c2));
}

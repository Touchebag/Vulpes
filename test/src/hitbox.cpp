#include <gtest/gtest.h>

#include "components/collision.h"
#include "components/hitbox.h"
#include "components/transform.h"

class StaticCollisionTestFixture : public ::testing::Test {
  public:
    void SetUp() {
        trans1_ = std::make_shared<Transform>();
        trans2_ = std::make_shared<Transform>();
        hbox1_ = std::make_shared<Hitbox>();
        hbox2_ = std::make_shared<Hitbox>();
    }

    std::shared_ptr<Transform> trans1_;
    std::shared_ptr<Transform> trans2_;
    std::shared_ptr<Hitbox> hbox1_;
    std::shared_ptr<Hitbox> hbox2_;
};

TEST_F(StaticCollisionTestFixture, CollideAtOrigin) {
    std::shared_ptr<Collision> c1 = std::make_shared<Collision>(trans1_, hbox1_);
    std::shared_ptr<Collision> c2 = std::make_shared<Collision>(trans2_, hbox2_);

    hbox1_->setHitbox(10, 20);
    hbox2_->setHitbox(5, 30);

    EXPECT_TRUE(c1->collides(c2));
}

TEST_F(StaticCollisionTestFixture, CollideAtOffset) {
    std::shared_ptr<Collision> c1 = std::make_shared<Collision>(trans1_, hbox1_);
    std::shared_ptr<Collision> c2 = std::make_shared<Collision>(trans2_, hbox2_);

    hbox1_->setHitbox(10, 20);
    trans1_->setPosition(5, 10);
    hbox2_->setHitbox(20, 30);
    trans2_->setPosition(5, 20);

    EXPECT_TRUE(c1->collides(c2));
}

TEST_F(StaticCollisionTestFixture, NotColliding) {
    std::shared_ptr<Collision> c1 = std::make_shared<Collision>(trans1_, hbox1_);
    std::shared_ptr<Collision> c2 = std::make_shared<Collision>(trans2_, hbox2_);

    hbox1_->setHitbox(10, 10);
    trans1_->setPosition(20, 50);
    hbox2_->setHitbox(10, 10);
    trans2_->setPosition(-40, 10);

    EXPECT_FALSE(c1->collides(c2));
}

TEST_F(StaticCollisionTestFixture, NotCollidingTouching) {
    std::shared_ptr<Collision> c1 = std::make_shared<Collision>(trans1_, hbox1_);
    std::shared_ptr<Collision> c2 = std::make_shared<Collision>(trans2_, hbox2_);

    hbox1_->setHitbox(10, 10);
    trans1_->setPosition(5, 5);
    hbox2_->setHitbox(10, 10);
    trans2_->setPosition(-5, -5);

    EXPECT_FALSE(c1->collides(c2));
}

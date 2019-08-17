#include <gtest/gtest.h>

#include "attributes/movable.h"

class MovableTestFixture : public ::testing::Test {
  public:
    MovableTestFixture() {
        trans_ = std::make_shared<Transform>();
        hitbox_ = std::make_shared<Hitbox>();
        move_ = std::make_shared<MovableEntity>(trans_, hitbox_, coll_);
    }

    void SetUp() {
        trans_->setPosition(0, 0);
    }

    std::shared_ptr<Transform> trans_;
    std::shared_ptr<Hitbox> hitbox_;
    std::shared_ptr<MovableEntity> move_;

    // Not used
    std::shared_ptr<Collision> coll_ = nullptr;

};

TEST_F(MovableTestFixture, MoveFromOrigin) {
    move_->move(1, 2);

    EXPECT_EQ(1, trans_->getX());
    EXPECT_EQ(2, trans_->getY());
}

TEST_F(MovableTestFixture, MoveNegative) {
    move_->move(-3, -10);

    EXPECT_EQ(-3, trans_->getX());
    EXPECT_EQ(-10, trans_->getY());
}

TEST_F(MovableTestFixture, MoveArbitrary) {
    trans_->setPosition(23, 42);
    move_->move(13, -57);

    EXPECT_EQ(36, trans_->getX());
    EXPECT_EQ(-15, trans_->getY());
}

TEST_F(MovableTestFixture, MoveFloat) {
    move_->move(6.7, 4.2);

    EXPECT_EQ(6, trans_->getX());
    EXPECT_EQ(4, trans_->getY());

    trans_->setPosition(0, 0);
    move_->move(-2.7, -3.5);

    EXPECT_EQ(-2, trans_->getX());
    EXPECT_EQ(-3, trans_->getY());
}

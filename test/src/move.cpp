#include <gtest/gtest.h>

#include "components/movement.h"
#include "components/component_store.h"

class MovableTestFixture : public ::testing::Test {
  public:
    MovableTestFixture() {
        components_->setComponent<Transform>(std::make_shared<Transform>(components_));
        components_->setComponent<MovableEntity>(std::make_shared<MovableEntity>(components_));
    }

    void SetUp() override {
        components_->getComponent<Transform>()->setPosition(0, 0);
    }

    std::shared_ptr<ComponentStore> components_ = std::make_shared<ComponentStore>();
};

TEST_F(MovableTestFixture, MoveFromOrigin) {
    components_->getComponent<MovableEntity>()->move(1, 2);

    EXPECT_EQ(1, components_->getComponent<Transform>()->getX());
    EXPECT_EQ(2, components_->getComponent<Transform>()->getY());
}

TEST_F(MovableTestFixture, MoveNegative) {
    components_->getComponent<MovableEntity>()->move(-3, -10);

    EXPECT_EQ(-3, components_->getComponent<Transform>()->getX());
    EXPECT_EQ(-10, components_->getComponent<Transform>()->getY());
}

TEST_F(MovableTestFixture, MoveArbitrary) {
    components_->getComponent<Transform>()->setPosition(23, 42);
    components_->getComponent<MovableEntity>()->move(13, -57);

    EXPECT_EQ(36, components_->getComponent<Transform>()->getX());
    EXPECT_EQ(-15, components_->getComponent<Transform>()->getY());
}

TEST_F(MovableTestFixture, MoveFloat) {
    components_->getComponent<MovableEntity>()->move(6.7, 4.2);

    EXPECT_EQ(6, components_->getComponent<Transform>()->getX());
    EXPECT_EQ(4, components_->getComponent<Transform>()->getY());

    components_->getComponent<Transform>()->setPosition(0, 0);
    components_->getComponent<MovableEntity>()->move(-2.7, -3.5);

    EXPECT_EQ(-2, components_->getComponent<Transform>()->getX());
    EXPECT_EQ(-3, components_->getComponent<Transform>()->getY());
}

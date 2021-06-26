#include <gtest/gtest.h>

#include "system/system.h"

class EnvironmentTestFixture : public ::testing::Test {
  public:
    EnvironmentTestFixture() {
    }

    void SetUp() override {
        // Clear everything
        System::IWorldModify::clearWorld();

        auto env = std::make_shared<Environment>();
        System::setEnvironment(env);
    }

    bool isConditionSet(std::string condition) {
        return System::getEnvironment()->getFlag(condition);
    }

    std::shared_ptr<BaseEntity> entity_;
};

TEST_F(EnvironmentTestFixture, AddComponentOnCondition) {
    std::string condition = "testCondition";

    // Ensure unset before test
    ASSERT_FALSE(isConditionSet(condition));

    ASSERT_EQ(System::IWorldModify::getWorldObjects().size(), 0);

    auto entity = BaseEntity::createFromJson({});
    System::IWorldModify::addEntity(entity, condition);

    // Should still be zero as condition is not met
    EXPECT_EQ(System::IWorldModify::getWorldObjects().size(), 0);

    System::getEnvironment()->setFlag(condition);

    // Should not happen before next frame update
    EXPECT_EQ(System::IWorldModify::getWorldObjects().size(), 0);

    System::IWorldModify::update();

    // Should now be added to main object list
    EXPECT_EQ(System::IWorldModify::getWorldObjects().size(), 1);
}

TEST_F(EnvironmentTestFixture, AddMultipleComponents) {
    std::string condition = "testCondition";
    std::string condition2 = "testCondition2";

    // Ensure unset before test
    ASSERT_FALSE(isConditionSet(condition));
    ASSERT_FALSE(isConditionSet(condition2));

    ASSERT_EQ(System::IWorldModify::getWorldObjects().size(), 0);

    auto entity = BaseEntity::createFromJson({});
    System::IWorldModify::addEntity(entity, condition);
    System::IWorldModify::addEntity(entity, condition);
    System::IWorldModify::addEntity(entity, condition2);

    System::getEnvironment()->setFlag(condition);
    System::IWorldModify::update();
    EXPECT_EQ(System::IWorldModify::getWorldObjects().size(), 2);

    System::getEnvironment()->setFlag(condition2);
    System::IWorldModify::update();
    EXPECT_EQ(System::IWorldModify::getWorldObjects().size(), 3);
}

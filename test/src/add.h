#include <gtest/gtest.h>

#include "command.h"

class AddTestFixture : public ::testing::Test {
  public:
    AddTestFixture() {
    }

    void SetUp() {
        std::shared_ptr<History> history = std::make_shared<History>();
        std::shared_ptr<Operation> operation = std::make_shared<Operation>();
        std::shared_ptr<Mouse> mouse = std::make_shared<Mouse>(window);
        std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();

        command_ = {history, entity, operation, mouse};

        World::getInstance().loadWorldFromJson(nlohmann::json::parse("{\"main\": null}"));
    }

  protected:
    sf::RenderWindow window;
    Command command_ = {nullptr, nullptr, nullptr, nullptr};
};

TEST_F(AddTestFixture, AddObject) {
    nlohmann::json world_json;

    auto& world = World::getInstance();

    ASSERT_TRUE(world.getWorldObjects(World::Layer::MAIN).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::MAIN_BG).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::MAIN_FG).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BG_1).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BG_2).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BG_3).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::FG_1).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::FG_2).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::FG_3).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BACKGROUND).size() == 0);

    command_.add();
    command_.add();

    command_.current_layer_ = World::Layer::FG_1;

    command_.add();

    ASSERT_TRUE(world.getWorldObjects(World::Layer::MAIN).size() == 2);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::MAIN_BG).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::MAIN_FG).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BG_1).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BG_2).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BG_3).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::FG_1).size() == 1);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::FG_2).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::FG_3).size() == 0);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BACKGROUND).size() == 0);
}

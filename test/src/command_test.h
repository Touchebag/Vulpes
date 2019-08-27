#include <gtest/gtest.h>

#include "command.h"

class CommandTestFixture : public ::testing::Test {
  public:
    CommandTestFixture() {
    }

    void SetUp() {
        std::shared_ptr<History> history = std::make_shared<History>();
        std::shared_ptr<Operation> operation = std::make_shared<Operation>();
        std::shared_ptr<Mouse> mouse = std::make_shared<Mouse>(window);

        entity_ = std::make_shared<BaseEntity>();
        command_ = {history, entity_, operation, mouse};

        World::getInstance().loadWorldFromJson(nlohmann::json::parse("{\"main\": null}"));
    }

  protected:
    sf::RenderWindow window;
    std::shared_ptr<BaseEntity> entity_;
    Command command_ = {nullptr, nullptr, nullptr, nullptr};
};

void test() {
    ASSERT_TRUE(false);
}

void assertCorrectNumberOfEntities(
        long long unsigned background,
        long long unsigned bg3,
        long long unsigned bg2,
        long long unsigned bg1,
        long long unsigned main_bg,
        long long unsigned main,
        long long unsigned main_fg,
        long long unsigned fg1,
        long long unsigned fg2,
        long long unsigned fg3) {
    auto& world = World::getInstance();
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BACKGROUND).size() == background);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BG_3).size() == bg3);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BG_2).size() == bg2);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::BG_1).size() == bg1);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::MAIN_BG).size() == main_bg);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::MAIN).size() == main);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::MAIN_FG).size() == main_fg);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::FG_1).size() == fg1);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::FG_2).size() == fg2);
    ASSERT_TRUE(world.getWorldObjects(World::Layer::FG_3).size() == fg3);
}

void assertWorldEmpty() {
    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

TEST_F(CommandTestFixture, AddObject) {
    assertWorldEmpty();

    command_.add();
    command_.add();

    command_.current_layer_ = World::Layer::FG_1;

    command_.add();

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 2, 0, 1, 0, 0);
}

TEST_F(CommandTestFixture, RemoveObject) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();

    command_.add();
    command_.add();
    command_.add(entity);

    command_.current_layer_ = World::Layer::BG_3;

    command_.add();

    assertCorrectNumberOfEntities(0, 1, 0, 0, 0, 3, 0, 0, 0, 0);

    command_.current_layer_ = World::Layer::MAIN;
    command_.remove(entity);

    assertCorrectNumberOfEntities(0, 1, 0, 0, 0, 2, 0, 0, 0, 0);
}

TEST_F(CommandTestFixture, CopyObjectRemoveOriginal) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();

    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.copy(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 2, 0, 0, 0, 0);

    command_.remove(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);
}

TEST_F(CommandTestFixture, CopyObjectCheckEqual) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();

    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.copy(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 2, 0, 0, 0, 0);

    nlohmann::json j = World::getInstance().saveWorldToJson();

    ASSERT_EQ(2, j["main"].size());

    EXPECT_TRUE(j["main"][0] == j["main"][1]);
}

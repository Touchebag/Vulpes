#include <gtest/gtest.h>

#include "command.h"
#include "mocks/mock_mouse.h"

class CommandTestFixture : public ::testing::Test {
  public:
    CommandTestFixture() {
    }

    void SetUp() {
        std::shared_ptr<History> history = std::make_shared<History>();
        std::shared_ptr<Operation> operation = std::make_shared<Operation>();

        mouse_ = std::make_shared<Mouse>(window_);

        entity_ = std::make_shared<BaseEntity>();
        command_ = {history, operation, mouse_};

        World::getInstance().loadWorldFromJson(nlohmann::json::parse("{\"main\": null}"));

        MockMouse::setMouseWorldPosition({0, 0});
    }

  protected:
    sf::RenderWindow window_;
    std::shared_ptr<BaseEntity> entity_;
    std::shared_ptr<Mouse> mouse_;
    Command command_ = {nullptr, nullptr, nullptr};
};

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

TEST_F(CommandTestFixture, ResizeObjectl) {
    assertWorldEmpty();

    MockMouse::setMouseWorldPosition({0, 0});

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);
    entity->trans_->setPosition(0, 0);
    mouse_->saveMousePosition();

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.startCommand(Command::Commands::RESIZE);

    MockMouse::setMouseWorldPosition({10, 9});
    command_.update();
    command_.stopCommand();

    window_.close();

    ASSERT_EQ(entity->hitbox_->width_, 70);
    ASSERT_EQ(entity->hitbox_->height_, 68);
}

TEST_F(CommandTestFixture, MoveObjectl) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    MockMouse::setMouseWorldPosition({10, 10});

    mouse_->saveMousePosition();

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.startCommand(Command::Commands::MOVE);

    MockMouse::setMouseWorldPosition({15, 27});
    command_.update();
    command_.stopCommand();

    window_.close();

    ASSERT_EQ(entity->trans_->getX(), 5);
    ASSERT_EQ(entity->trans_->getY(), 17);
}

TEST_F(CommandTestFixture, ToggleRenderable) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.startCommand(Command::Commands::TOGGLE_RENDERABLE);

    ASSERT_FALSE(entity->renderableEntity_);

    command_.startCommand(Command::Commands::TOGGLE_RENDERABLE);
    ASSERT_TRUE(entity->renderableEntity_);
}

TEST_F(CommandTestFixture, ToggleCollision) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->collision_ = std::make_shared<Collision>(entity->trans_, entity->hitbox_);
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.startCommand(Command::Commands::TOGGLE_COLLISION);

    ASSERT_FALSE(entity->collision_);

    command_.startCommand(Command::Commands::TOGGLE_COLLISION);
    ASSERT_TRUE(entity->collision_);
}

TEST_F(CommandTestFixture, ToggleMovable) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->movableEntity_ = std::make_shared<MovableEntity>(entity->trans_, entity->hitbox_, entity->collision_);
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.startCommand(Command::Commands::TOGGLE_MOVABLE);

    ASSERT_FALSE(entity->movableEntity_);

    command_.startCommand(Command::Commands::TOGGLE_MOVABLE);
    ASSERT_TRUE(entity->movableEntity_);
}

TEST_F(CommandTestFixture, TogglePhysics) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->physics_ = std::make_shared<Physics>(
                           entity->statefulEntity_,
                           entity->renderableEntity_,
                           entity->movableEntity_,
                           entity->animatedEntity_,
                           entity->actions_,
                           entity->collision_);
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.startCommand(Command::Commands::TOGGLE_PHYSICS);

    ASSERT_FALSE(entity->physics_);

    command_.startCommand(Command::Commands::TOGGLE_PHYSICS);
    ASSERT_TRUE(entity->physics_);
}

TEST_F(CommandTestFixture, ToggleActions) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->actions_ = std::make_shared<Actions>();
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.startCommand(Command::Commands::TOGGLE_ACTIONS);

    ASSERT_FALSE(entity->actions_);

    command_.startCommand(Command::Commands::TOGGLE_ACTIONS);
    ASSERT_TRUE(entity->actions_);
}

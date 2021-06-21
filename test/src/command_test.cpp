#include <gtest/gtest.h>

#include "level_editor/editor_loop/editor_environment.h"
#include "mocks/mock_mouse.h"

#include "components/collision/collideables/movement/collideable_static.h"

class CommandTestFixture : public ::testing::Test {
  public:
    CommandTestFixture() {
    }

    void SetUp() override {
        std::shared_ptr<Operation> operation = std::make_shared<Operation>();

        System::IWorldModify::loadWorldFromJson(nlohmann::json::parse("{\"entities\": null}"));

        MockMouse::setMouseWorldPosition({0, 0});
    }

  protected:
    sf::RenderWindow window_;
    std::shared_ptr<EditorEnvironment> editor_env = EditorEnvironment::create_environment(window_);
};

void assertCorrectNumberOfEntities(long long unsigned number) {
    auto world_objects = System::IWorldModify::getWorldObjects();

    ASSERT_EQ(world_objects.size(), number);
}

void assertWorldEmpty() {
    assertCorrectNumberOfEntities(0);
}

TEST_F(CommandTestFixture, AddObject) {
    assertWorldEmpty();

    editor_env->command->add();
    editor_env->command->add();

    editor_env->command->current_layer_ = 1;

    editor_env->command->add();

    assertCorrectNumberOfEntities(3);
}

TEST_F(CommandTestFixture, RemoveObject) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();

    editor_env->command->add();
    editor_env->command->add();
    editor_env->command->add(entity);

    editor_env->command->current_layer_ = -3;

    editor_env->command->add();

    assertCorrectNumberOfEntities(4);

    editor_env->command->current_layer_ = 0;
    editor_env->command->remove(entity);

    assertCorrectNumberOfEntities(3);
}

TEST_F(CommandTestFixture, CopyObjectRemoveOriginal) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();

    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->command->copy(entity);

    assertCorrectNumberOfEntities(2);

    editor_env->command->remove(entity);

    assertCorrectNumberOfEntities(1);
}

TEST_F(CommandTestFixture, CopyObjectCheckEqual) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();

    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->command->copy(entity);

    assertCorrectNumberOfEntities(2);

    nlohmann::json j = System::IWorldModify::saveWorldToJson();

    ASSERT_EQ(2, j["entities"].size());

    EXPECT_TRUE(j["entities"][0] == j["entities"][1]);
}

TEST_F(CommandTestFixture, ResizeObject) {
    assertWorldEmpty();

    MockMouse::setMouseWorldPosition({0, 0});

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);
    entity->getComponent<Transform>()->setPosition(0, 0);
    entity->setComponent<Collision>(std::make_shared<Collision>(entity->components_));
    entity->getComponent<Collision>()->getCollideable()->setHitbox(50, 50);

    editor_env->mouse->saveMousePosition();

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::RESIZE);

    MockMouse::setMouseWorldPosition({10, 9});
    editor_env->command->update();
    editor_env->command->stopCommand();

    window_.close();

    ASSERT_EQ(entity->getComponent<Collision>()->getCollideable()->getHitbox()->width_, 70);
    ASSERT_EQ(entity->getComponent<Collision>()->getCollideable()->getHitbox()->height_, 68);
}

TEST_F(CommandTestFixture, MoveObject) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    MockMouse::setMouseWorldPosition({10, 10});

    editor_env->mouse->saveMousePosition();

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::MOVE);

    MockMouse::setMouseWorldPosition({15, 27});
    editor_env->command->update();
    editor_env->command->stopCommand();

    window_.close();

    ASSERT_EQ(entity->getComponent<Transform>()->getX(), 5);
    ASSERT_EQ(entity->getComponent<Transform>()->getY(), 17);
}

TEST_F(CommandTestFixture, ToggleRendering) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);
    ASSERT_TRUE(entity->getComponent<Rendering>());

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_RENDERABLE);

    ASSERT_FALSE(entity->getComponent<Rendering>());

    editor_env->command->handleCommand(Command::Commands::TOGGLE_RENDERABLE);
    ASSERT_TRUE(entity->getComponent<Rendering>());
}

TEST_F(CommandTestFixture, ToggleCollision) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->setComponent<Collision>(std::make_shared<Collision>(entity->components_));
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_COLLISION);

    ASSERT_FALSE(entity->getComponent<Collision>());

    editor_env->command->handleCommand(Command::Commands::TOGGLE_COLLISION);
    ASSERT_TRUE(entity->getComponent<Collision>());
}

TEST_F(CommandTestFixture, ToggleMovement) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->setComponent<Movement>(std::make_shared<Movement>(entity->components_));
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_MOVABLE);

    ASSERT_FALSE(entity->getComponent<Movement>());

    editor_env->command->handleCommand(Command::Commands::TOGGLE_MOVABLE);
    ASSERT_TRUE(entity->getComponent<Movement>());
}

TEST_F(CommandTestFixture, TogglePhysics) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->setComponent<Physics>(std::make_shared<Physics>(entity->components_));
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_PHYSICS);

    ASSERT_FALSE(entity->getComponent<Physics>());

    editor_env->command->handleCommand(Command::Commands::TOGGLE_PHYSICS);
    ASSERT_TRUE(entity->getComponent<Physics>());
}

TEST_F(CommandTestFixture, ToggleActions) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->setComponent<Actions>(std::make_shared<Actions>(entity->components_));
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_ACTIONS);

    ASSERT_FALSE(entity->getComponent<Actions>());

    editor_env->command->handleCommand(Command::Commands::TOGGLE_ACTIONS);
    ASSERT_TRUE(entity->getComponent<Actions>());
}

TEST_F(CommandTestFixture, ToggleTiling) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    entity->setComponent<Rendering>(std::make_shared<Rendering>(entity->components_));

    ASSERT_EQ(0, entity->getComponent<Rendering>()->tiling_x_);
    ASSERT_EQ(0, entity->getComponent<Rendering>()->tiling_y_);

    editor_env->current_entity = entity;

    editor_env->command->handleCommand(Command::Commands::RENDERABLE_TILING_X);
    editor_env->command->text_input_->enterText("1");
    editor_env->command->stopCommand();

    ASSERT_EQ(1, entity->getComponent<Rendering>()->tiling_x_);
    ASSERT_EQ(0, entity->getComponent<Rendering>()->tiling_y_);

    editor_env->command->handleCommand(Command::Commands::RENDERABLE_TILING_Y);
    editor_env->command->text_input_->enterText("2");
    editor_env->command->stopCommand();

    ASSERT_EQ(1, entity->getComponent<Rendering>()->tiling_x_);
    ASSERT_EQ(2, entity->getComponent<Rendering>()->tiling_y_);
}

TEST_F(CommandTestFixture, ChangeTexture) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;

    ASSERT_EQ(entity->outputToJson().value()["Rendering"]["texture"].get<std::string>(), "box.png");

    editor_env->command->handleCommand(Command::Commands::RENDERABLE_SPRITE_CHANGE);

    editor_env->command->text_input_->enterText("box_bg1.png");

    editor_env->command->stopCommand();

    ASSERT_EQ(entity->outputToJson().value()["Rendering"]["texture"].get<std::string>(), "box_bg1.png");

    // Actual input will arrive character by character
    editor_env->command->handleCommand(Command::Commands::RENDERABLE_SPRITE_CHANGE);

    editor_env->command->text_input_->enterText("b");
    editor_env->command->text_input_->enterText("o");
    editor_env->command->text_input_->enterText("x");
    editor_env->command->text_input_->enterText(".");
    editor_env->command->text_input_->enterText("p");
    editor_env->command->text_input_->enterText("n");
    editor_env->command->text_input_->enterText("g");

    editor_env->command->stopCommand();

    ASSERT_EQ(entity->outputToJson().value()["Rendering"]["texture"].get<std::string>(), "box.png");
}

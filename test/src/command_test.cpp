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

        World::getInstance<World::IWorldModify>().loadWorldFromJson(nlohmann::json::parse("{\"entities\": null}"));

        MockMouse::setMouseWorldPosition({0, 0});
    }

  protected:
    sf::RenderWindow window_;
    std::shared_ptr<EditorEnvironment> editor_env = EditorEnvironment::create_environment(window_);
};

void assertCorrectNumberOfEntities(long long unsigned number) {
    auto world_objects = World::getInstance<World::IWorldModify>().getWorldObjects();

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

    nlohmann::json j = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_EQ(2, j["entities"].size());

    EXPECT_TRUE(j["entities"][0] == j["entities"][1]);
}

TEST_F(CommandTestFixture, ResizeObject) {
    assertWorldEmpty();

    MockMouse::setMouseWorldPosition({0, 0});

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);
    entity->trans_->setPosition(0, 0);
    entity->collision_ = std::make_shared<Collision>(entity->trans_, entity->actions_);
    entity->collision_->getCollideable()->setHitbox(50, 50);

    editor_env->mouse->saveMousePosition();

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::RESIZE);

    MockMouse::setMouseWorldPosition({10, 9});
    editor_env->command->update();
    editor_env->command->stopCommand();

    window_.close();

    ASSERT_EQ(entity->collision_->getCollideable()->getHitbox()->width_, 70);
    ASSERT_EQ(entity->collision_->getCollideable()->getHitbox()->height_, 68);
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

    ASSERT_EQ(entity->trans_->getX(), 5);
    ASSERT_EQ(entity->trans_->getY(), 17);
}

TEST_F(CommandTestFixture, ToggleRenderable) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);
    ASSERT_TRUE(entity->renderableEntity_);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_RENDERABLE);

    ASSERT_FALSE(entity->renderableEntity_);

    editor_env->command->handleCommand(Command::Commands::TOGGLE_RENDERABLE);
    ASSERT_TRUE(entity->renderableEntity_);
}

TEST_F(CommandTestFixture, ToggleCollision) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->collision_ = std::make_shared<Collision>(entity->trans_, entity->actions_);
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_COLLISION);

    ASSERT_FALSE(entity->collision_);

    editor_env->command->handleCommand(Command::Commands::TOGGLE_COLLISION);
    ASSERT_TRUE(entity->collision_);
}

TEST_F(CommandTestFixture, ToggleMovable) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->movableEntity_ = std::make_shared<MovableEntity>(entity->trans_, entity->collision_);
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_MOVABLE);

    ASSERT_FALSE(entity->movableEntity_);

    editor_env->command->handleCommand(Command::Commands::TOGGLE_MOVABLE);
    ASSERT_TRUE(entity->movableEntity_);
}

TEST_F(CommandTestFixture, TogglePhysics) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->physics_ = std::make_shared<Physics>(
                           entity->statefulEntity_,
                           entity->movableEntity_,
                           entity->animatedEntity_,
                           entity->actions_);
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_PHYSICS);

    ASSERT_FALSE(entity->physics_);

    editor_env->command->handleCommand(Command::Commands::TOGGLE_PHYSICS);
    ASSERT_TRUE(entity->physics_);
}

TEST_F(CommandTestFixture, ToggleActions) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->actions_ = std::make_shared<Actions>(entity->death_);
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_ACTIONS);

    ASSERT_FALSE(entity->actions_);

    editor_env->command->handleCommand(Command::Commands::TOGGLE_ACTIONS);
    ASSERT_TRUE(entity->actions_);
}

TEST_F(CommandTestFixture, ToggleTiling) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    entity->renderableEntity_ = std::make_shared<RenderableEntity>(entity->trans_, entity->movableEntity_);

    ASSERT_EQ(0, entity->renderableEntity_->tiling_x_);
    ASSERT_EQ(0, entity->renderableEntity_->tiling_y_);

    editor_env->current_entity = entity;

    editor_env->command->handleCommand(Command::Commands::RENDERABLE_TILING_X);
    editor_env->command->text_input_->enterText("1");
    editor_env->command->stopCommand();

    ASSERT_EQ(1, entity->renderableEntity_->tiling_x_);
    ASSERT_EQ(0, entity->renderableEntity_->tiling_y_);

    editor_env->command->handleCommand(Command::Commands::RENDERABLE_TILING_Y);
    editor_env->command->text_input_->enterText("2");
    editor_env->command->stopCommand();

    ASSERT_EQ(1, entity->renderableEntity_->tiling_x_);
    ASSERT_EQ(2, entity->renderableEntity_->tiling_y_);
}

TEST_F(CommandTestFixture, ChangeTexture) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;

    ASSERT_EQ(entity->outputToJson().value()["Renderable"]["texture"].get<std::string>(), "box.png");

    editor_env->command->handleCommand(Command::Commands::RENDERABLE_SPRITE_CHANGE);

    editor_env->command->text_input_->enterText("box_bg1.png");

    editor_env->command->stopCommand();

    ASSERT_EQ(entity->outputToJson().value()["Renderable"]["texture"].get<std::string>(), "box_bg1.png");

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

    ASSERT_EQ(entity->outputToJson().value()["Renderable"]["texture"].get<std::string>(), "box.png");
}

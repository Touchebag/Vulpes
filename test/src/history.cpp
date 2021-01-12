#include <gtest/gtest.h>

#include "level_editor/editor_loop/editor_environment.h"
#include "level_editor/history.h"
#include "mocks/mock_mouse.h"

#include "components/collision/collideables/movement/collideable_static.h"

class HistoryTestFixture : public ::testing::Test {
  public:
    HistoryTestFixture() {
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

TEST_F(HistoryTestFixture, AddObject) {
    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->command->add();
    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, RemoveObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->command->remove(entity);
    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3) << j1.dump() << std::endl << j3.dump() << std::endl;
    ASSERT_TRUE(j2 == j4) << j2.dump() << std::endl << j4.dump() << std::endl;
}

TEST_F(HistoryTestFixture, CopyObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->command->copy(entity);
    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, CopyObjectRemoveOriginal) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->command->copy(entity);
    editor_env->command->remove(entity);
    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ResizeObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    MockMouse::setMouseWorldPosition({0, 0});
    entity->trans_->setPosition(0, 0);
    editor_env->mouse->saveMousePosition();

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::RESIZE);

    MockMouse::setMouseWorldPosition({10, 9});
    editor_env->command->update();
    editor_env->command->stopCommand();

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, MoveObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    MockMouse::setMouseWorldPosition({0, 0});
    entity->trans_->setPosition(0, 0);
    editor_env->mouse->saveMousePosition();

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::MOVE);

    MockMouse::setMouseWorldPosition({10, 9});
    editor_env->command->update();
    editor_env->command->stopCommand();

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, MoveObjectOtherLayer) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->current_layer_ = RenderableEntity::Layer::BG_1;
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    MockMouse::setMouseWorldPosition({0, 0});
    entity->trans_->setPosition(0, 0);
    editor_env->mouse->saveMousePosition();

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::MOVE);

    MockMouse::setMouseWorldPosition({10, 9});
    editor_env->command->update();
    editor_env->command->stopCommand();

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ToggleRenderable) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_RENDERABLE);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ToggleCollision) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->collision_ = std::make_shared<Collision>(entity->trans_);
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_COLLISION);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ToggleMovable) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->movableEntity_ = std::make_shared<MovableEntity>(entity->trans_, entity->collision_);
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_MOVABLE);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, TogglePhysics) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->physics_ = std::make_shared<Physics>(
                           entity->statefulEntity_,
                           entity->movableEntity_,
                           entity->animatedEntity_,
                           entity->actions_);
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_PHYSICS);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ToggleActions) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->actions_ = std::make_shared<Actions>(entity->death_);
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::TOGGLE_ACTIONS);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3) << j1.dump() << std::endl << j3.dump() << std::endl;
    ASSERT_TRUE(j2 == j4) << j2.dump() << std::endl << j4.dump() << std::endl;
}

TEST_F(HistoryTestFixture, ToggleTiling) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->renderableEntity_ = std::make_shared<RenderableEntity>(entity->trans_, entity->movableEntity_);
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::RENDERABLE_TILING_NONE);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ChangeSprite) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->renderableEntity_ = std::make_shared<RenderableEntity>(entity->trans_, entity->movableEntity_);
    editor_env->command->add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->current_entity = entity;
    editor_env->command->handleCommand(Command::Commands::RENDERABLE_SPRITE_CHANGE);

    editor_env->command->text_input_->enterText("box_bg1.png");

    editor_env->command->stopCommand();

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    editor_env->history->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}


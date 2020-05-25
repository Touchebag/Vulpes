#include <gtest/gtest.h>

#include "level_editor/command.h"
#include "mocks/mock_mouse.h"

class HistoryTestFixture : public ::testing::Test {
  public:
    HistoryTestFixture() {
    }

    void SetUp() override {
        std::shared_ptr<Operation> operation = std::make_shared<Operation>();

        history_ = std::make_shared<History>();
        mouse_ = std::make_shared<Mouse>(window_);

        entity_ = std::make_shared<BaseEntity>();
        command_ = {history_, operation, mouse_};

        World::getInstance<World::IWorldModify>().loadWorldFromJson(nlohmann::json::parse("{\"main\": null}"));

        MockMouse::setMouseWorldPosition({0, 0});
    }

  protected:
    sf::RenderWindow window_;
    std::shared_ptr<BaseEntity> entity_;
    std::shared_ptr<Mouse> mouse_;
    std::shared_ptr<History> history_;
    Command command_ = {nullptr, nullptr, nullptr};
};

TEST_F(HistoryTestFixture, AddObject) {
    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.add();
    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, RemoveObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.remove(entity);
    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, CopyObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.copy(entity);
    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, CopyObjectRemoveOriginal) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.copy(entity);
    command_.remove(entity);
    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ResizeObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    MockMouse::setMouseWorldPosition({0, 0});
    entity->trans_->setPosition(0, 0);
    mouse_->saveMousePosition();

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::RESIZE);

    MockMouse::setMouseWorldPosition({10, 9});
    command_.update();
    command_.stopCommand();

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, MoveObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    MockMouse::setMouseWorldPosition({0, 0});
    entity->trans_->setPosition(0, 0);
    mouse_->saveMousePosition();

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::MOVE);

    MockMouse::setMouseWorldPosition({10, 9});
    command_.update();
    command_.stopCommand();

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, MoveObjectOtherLayer) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.current_layer_ = World::Layer::BG_1;
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    MockMouse::setMouseWorldPosition({0, 0});
    entity->trans_->setPosition(0, 0);
    mouse_->saveMousePosition();

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::MOVE);

    MockMouse::setMouseWorldPosition({10, 9});
    command_.update();
    command_.stopCommand();

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ToggleRenderable) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::TOGGLE_RENDERABLE);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ToggleCollision) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->collision_ = std::make_shared<Collision>(entity->trans_, entity->hitbox_);
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::TOGGLE_COLLISION);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ToggleMovable) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->movableEntity_ = std::make_shared<MovableEntity>(entity->trans_, entity->hitbox_, entity->collision_);
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::TOGGLE_MOVABLE);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, TogglePhysics) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->physics_ = std::make_shared<Physics>(
                           entity->statefulEntity_,
                           entity->renderableEntity_,
                           entity->movableEntity_,
                           entity->animatedEntity_,
                           entity->actions_,
                           entity->collision_);
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::TOGGLE_PHYSICS);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ToggleActions) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->actions_ = std::make_shared<Actions>(entity->death_);
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::TOGGLE_ACTIONS);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ToggleTiling) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->renderableEntity_ = std::make_shared<RenderableEntity>(entity->trans_);
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::RENDERABLE_TILING_NONE);

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ChangeSprite) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->renderableEntity_ = std::make_shared<RenderableEntity>(entity->trans_);
    command_.add(entity);

    auto j1 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::RENDERABLE_SPRITE_CHANGE);

    command_.text_input_->enterText("box_bg1.png");

    command_.stopCommand();

    auto j2 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}


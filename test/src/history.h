#include <gtest/gtest.h>

#include "command.h"
#include "mocks/mock_mouse.h"

class HistoryTestFixture : public ::testing::Test {
  public:
    HistoryTestFixture() {
    }

    void SetUp() {
        std::shared_ptr<Operation> operation = std::make_shared<Operation>();

        history_ = std::make_shared<History>();
        mouse_ = std::make_shared<Mouse>(window_);

        entity_ = std::make_shared<BaseEntity>();
        command_ = {history_, operation, mouse_};

        World::getInstance().loadWorldFromJson(nlohmann::json::parse("{\"main\": null}"));

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
    auto j1 = World::getInstance().saveWorldToJson();

    command_.add();
    auto j2 = World::getInstance().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, RemoveObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    auto j1 = World::getInstance().saveWorldToJson();

    command_.remove(entity);
    auto j2 = World::getInstance().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, CopyObjectRemoveOriginal) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    auto j1 = World::getInstance().saveWorldToJson();

    command_.copy(entity);
    auto j2 = World::getInstance().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, ResizeObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    auto j1 = World::getInstance().saveWorldToJson();

    MockMouse::setMouseWorldPosition({0, 0});
    entity->trans_->setPosition(0, 0);
    mouse_->saveMousePosition();

    command_.current_entity_ = entity;
    command_.startCommand(Command::Commands::RESIZE);

    MockMouse::setMouseWorldPosition({10, 9});
    command_.update();
    command_.stopCommand();

    auto j2 = World::getInstance().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, MoveObject) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    auto j1 = World::getInstance().saveWorldToJson();

    MockMouse::setMouseWorldPosition({0, 0});
    entity->trans_->setPosition(0, 0);
    mouse_->saveMousePosition();

    command_.current_entity_ = entity;
    command_.startCommand(Command::Commands::MOVE);

    MockMouse::setMouseWorldPosition({10, 9});
    command_.update();
    command_.stopCommand();

    auto j2 = World::getInstance().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

TEST_F(HistoryTestFixture, MoveObjectOtherLayer) {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.current_layer_ = World::Layer::BG_1;
    command_.add(entity);

    auto j1 = World::getInstance().saveWorldToJson();

    MockMouse::setMouseWorldPosition({0, 0});
    entity->trans_->setPosition(0, 0);
    mouse_->saveMousePosition();

    command_.current_entity_ = entity;
    command_.startCommand(Command::Commands::MOVE);

    MockMouse::setMouseWorldPosition({10, 9});
    command_.update();
    command_.stopCommand();

    auto j2 = World::getInstance().saveWorldToJson();

    history_->undo();
    auto j3 = World::getInstance().saveWorldToJson();

    history_->redo();
    auto j4 = World::getInstance().saveWorldToJson();

    ASSERT_TRUE(j1 == j3);
    ASSERT_TRUE(j2 == j4);
}

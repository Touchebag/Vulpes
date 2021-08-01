#include <gtest/gtest.h>

#include "level_editor/editor_loop/editor_environment.h"
#include "mocks/mock_mouse.h"

#include "menus/menu_rendering.h"

#include "components/collision/collideables/movement/collideable_static.h"

class CommandTestFixture : public ::testing::Test {
  public:
    CommandTestFixture() {
    }

    void SetUp() override {
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

    editor_env->current_entity = entity;
    editor_env->command->current_layer_ = 0;
    editor_env->command->remove();

    assertCorrectNumberOfEntities(3);
}

TEST_F(CommandTestFixture, CopyObjectRemoveOriginal) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();

    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;

    editor_env->command->copy();
    editor_env->current_entity = nullptr;
    editor_env->command->paste();

    assertCorrectNumberOfEntities(2);

    editor_env->current_entity = entity;
    editor_env->command->remove();

    assertCorrectNumberOfEntities(1);
}

TEST_F(CommandTestFixture, CopyObjectCheckEqual) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();

    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    editor_env->current_entity = entity;
    editor_env->command->copy();

    editor_env->current_entity = nullptr;
    editor_env->command->paste();

    assertCorrectNumberOfEntities(2);

    nlohmann::json j = System::IWorldModify::saveWorldToJson();

    ASSERT_EQ(2, j["entities"].size());

    EXPECT_TRUE(j["entities"][0] == j["entities"][1]);
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

TEST_F(CommandTestFixture, ChangeTiling) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    editor_env->command->add(entity);

    assertCorrectNumberOfEntities(1);

    entity->setComponent<Rendering>(std::make_shared<Rendering>(entity->components_));

    ASSERT_EQ(0, entity->getComponent<Rendering>()->tiling_x_);
    ASSERT_EQ(0, entity->getComponent<Rendering>()->tiling_y_);

    editor_env->current_entity = entity;

    editor_env->menus.Rendering->setTiling(entity->getComponent<Rendering>(), 1, 0);

    ASSERT_EQ(1, entity->getComponent<Rendering>()->tiling_x_);
    ASSERT_EQ(0, entity->getComponent<Rendering>()->tiling_y_);

    editor_env->menus.Rendering->setTiling(entity->getComponent<Rendering>(), 1, 2);

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

    editor_env->menus.Rendering->setTexture(entity->getComponent<Rendering>(), "box_bg1.png");

    ASSERT_EQ(entity->outputToJson().value()["Rendering"]["texture"].get<std::string>(), "box_bg1.png");
}

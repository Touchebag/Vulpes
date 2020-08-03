#include <gtest/gtest.h>

#include "level_editor/command.h"
#include "mocks/mock_mouse.h"

#include "components/collision/movement/collision_static.h"

class CommandTestFixture : public ::testing::Test {
  public:
    CommandTestFixture() {
    }

    void SetUp() override {
        std::shared_ptr<History> history = std::make_shared<History>();
        std::shared_ptr<Operation> operation = std::make_shared<Operation>();

        mouse_ = std::make_shared<Mouse>(window_);

        entity_ = std::make_shared<BaseEntity>();
        command_ = {history, operation, mouse_};

        World::getInstance<World::IWorldModify>().loadWorldFromJson(nlohmann::json::parse("{\"entities\": null}"));

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
    long long unsigned count_background = 0;
    long long unsigned count_bg3 = 0;
    long long unsigned count_bg2 = 0;
    long long unsigned count_bg1 = 0;
    long long unsigned count_main_bg = 0;
    long long unsigned count_main = 0;
    long long unsigned count_main_fg = 0;
    long long unsigned count_fg1 = 0;
    long long unsigned count_fg2 = 0;
    long long unsigned count_fg3 = 0;

    auto world_objects = World::getInstance<World::IWorldModify>().getWorldObjects();

    for (auto it : world_objects) {
        switch (it->renderableEntity_->getLayer()) {
            case (RenderableEntity::Layer::BACKGROUND):
                count_background++;
                break;
            case (RenderableEntity::Layer::BG_3):
                count_bg3++;
                break;
            case (RenderableEntity::Layer::BG_2):
                count_bg2++;
                break;
            case (RenderableEntity::Layer::BG_1):
                count_bg1++;
                break;
            case (RenderableEntity::Layer::MAIN_BG):
                count_main_bg++;
                break;
            case (RenderableEntity::Layer::MAIN):
                count_main++;
                break;
            case (RenderableEntity::Layer::MAIN_FG):
                count_main_fg++;
                break;
            case (RenderableEntity::Layer::FG_1):
                count_fg1++;
                break;
            case (RenderableEntity::Layer::FG_2):
                count_fg2++;
                break;
            case (RenderableEntity::Layer::FG_3):
                count_fg3++;
                break;
            default:
                break;
        }
    }
    ASSERT_TRUE(count_background == background);
    ASSERT_TRUE(count_bg3 == bg3);
    ASSERT_TRUE(count_bg2 == bg2);
    ASSERT_TRUE(count_bg1 == bg1);
    ASSERT_TRUE(count_main_bg == main_bg);
    ASSERT_TRUE(count_main == main);
    ASSERT_TRUE(count_main_fg == main_fg);
    ASSERT_TRUE(count_fg1 == fg1);
    ASSERT_TRUE(count_fg2 == fg2);
    ASSERT_TRUE(count_fg3 == fg3);
}

void assertWorldEmpty() {
    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

TEST_F(CommandTestFixture, AddObject) {
    assertWorldEmpty();

    command_.add();
    command_.add();

    command_.current_layer_ = RenderableEntity::Layer::FG_1;

    command_.add();

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 2, 0, 1, 0, 0);
}

TEST_F(CommandTestFixture, RemoveObject) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();

    command_.add();
    command_.add();
    command_.add(entity);

    command_.current_layer_ = RenderableEntity::Layer::BG_3;

    command_.add();

    assertCorrectNumberOfEntities(0, 1, 0, 0, 0, 3, 0, 0, 0, 0);

    command_.current_layer_ = RenderableEntity::Layer::MAIN;
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

    nlohmann::json j = World::getInstance<World::IWorldModify>().saveWorldToJson();

    ASSERT_EQ(2, j["entities"].size());

    EXPECT_TRUE(j["entities"][0] == j["entities"][1]);
}

TEST_F(CommandTestFixture, ResizeObjectl) {
    assertWorldEmpty();

    MockMouse::setMouseWorldPosition({0, 0});

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);
    entity->trans_->setPosition(0, 0);
    entity->collision_ = std::make_shared<CollisionStatic>(entity->trans_);

    mouse_->saveMousePosition();

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::RESIZE);

    MockMouse::setMouseWorldPosition({10, 9});
    command_.update();
    command_.stopCommand();

    window_.close();

    ASSERT_EQ(entity->collision_->getHitbox()->width_, 70);
    ASSERT_EQ(entity->collision_->getHitbox()->height_, 68);
}

TEST_F(CommandTestFixture, MoveObjectl) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    MockMouse::setMouseWorldPosition({10, 10});

    mouse_->saveMousePosition();

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::MOVE);

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
    command_.handleCommand(Command::Commands::TOGGLE_RENDERABLE);

    ASSERT_FALSE(entity->renderableEntity_);

    command_.handleCommand(Command::Commands::TOGGLE_RENDERABLE);
    ASSERT_TRUE(entity->renderableEntity_);
}

TEST_F(CommandTestFixture, ToggleCollision) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->collision_ = std::make_shared<CollisionStatic>(entity->trans_);
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::TOGGLE_COLLISION);

    ASSERT_FALSE(entity->collision_);

    command_.handleCommand(Command::Commands::TOGGLE_COLLISION);
    ASSERT_TRUE(entity->collision_);
}

TEST_F(CommandTestFixture, ToggleMovable) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->movableEntity_ = std::make_shared<MovableEntity>(entity->trans_, entity->collision_);
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::TOGGLE_MOVABLE);

    ASSERT_FALSE(entity->movableEntity_);

    command_.handleCommand(Command::Commands::TOGGLE_MOVABLE);
    ASSERT_TRUE(entity->movableEntity_);
}

TEST_F(CommandTestFixture, TogglePhysics) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->physics_ = std::make_shared<Physics>(
                           entity->statefulEntity_,
                           entity->movableEntity_,
                           entity->animatedEntity_,
                           entity->actions_,
                           entity->collision_);
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::TOGGLE_PHYSICS);

    ASSERT_FALSE(entity->physics_);

    command_.handleCommand(Command::Commands::TOGGLE_PHYSICS);
    ASSERT_TRUE(entity->physics_);
}

TEST_F(CommandTestFixture, ToggleActions) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    entity->actions_ = std::make_shared<Actions>(entity->death_);
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::TOGGLE_ACTIONS);

    ASSERT_FALSE(entity->actions_);

    command_.handleCommand(Command::Commands::TOGGLE_ACTIONS);
    ASSERT_TRUE(entity->actions_);
}

TEST_F(CommandTestFixture, ToggleTiling) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    entity->renderableEntity_ = std::make_shared<RenderableEntity>(entity->trans_, entity->movableEntity_);

    ASSERT_TRUE(entity->renderableEntity_->tiling_x_);
    ASSERT_TRUE(entity->renderableEntity_->tiling_y_);

    command_.current_entity_ = entity;
    command_.handleCommand(Command::Commands::RENDERABLE_TILING_NONE);

    EXPECT_FALSE(entity->renderableEntity_->tiling_x_);
    EXPECT_FALSE(entity->renderableEntity_->tiling_y_);

    command_.handleCommand(Command::Commands::RENDERABLE_TILING_X);

    EXPECT_TRUE(entity->renderableEntity_->tiling_x_);
    EXPECT_FALSE(entity->renderableEntity_->tiling_y_);

    command_.handleCommand(Command::Commands::RENDERABLE_TILING_Y);

    EXPECT_FALSE(entity->renderableEntity_->tiling_x_);
    EXPECT_TRUE(entity->renderableEntity_->tiling_y_);

    command_.handleCommand(Command::Commands::RENDERABLE_TILING_XY);

    EXPECT_TRUE(entity->renderableEntity_->tiling_x_);
    EXPECT_TRUE(entity->renderableEntity_->tiling_y_);
}

TEST_F(CommandTestFixture, ChangeTexture) {
    assertWorldEmpty();

    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    command_.add(entity);

    assertCorrectNumberOfEntities(0, 0, 0, 0, 0, 1, 0, 0, 0, 0);

    command_.current_entity_ = entity;

    ASSERT_EQ(entity->outputToJson().value()["Renderable"]["texture"].get<std::string>(), "box.png");

    command_.handleCommand(Command::Commands::RENDERABLE_SPRITE_CHANGE);

    command_.text_input_->enterText("box_bg1.png");

    command_.stopCommand();

    ASSERT_EQ(entity->outputToJson().value()["Renderable"]["texture"].get<std::string>(), "box_bg1.png");

    // Actual input will arrive character by character
    command_.handleCommand(Command::Commands::RENDERABLE_SPRITE_CHANGE);

    command_.text_input_->enterText("b");
    command_.text_input_->enterText("o");
    command_.text_input_->enterText("x");
    command_.text_input_->enterText(".");
    command_.text_input_->enterText("p");
    command_.text_input_->enterText("n");
    command_.text_input_->enterText("g");

    command_.stopCommand();

    ASSERT_EQ(entity->outputToJson().value()["Renderable"]["texture"].get<std::string>(), "box.png");
}

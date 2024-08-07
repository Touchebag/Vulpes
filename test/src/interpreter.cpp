#include <gtest/gtest.h>

#include "components/component_store.h"
#include "components/scripting/program.h"
#include "components/rendering/rendering.h"
#include "system/system.h"

#include "utils/log.h"

using scripting::Bool;
using scripting::Target;

class InterpreterTestFixture : public ::testing::Test {
  public:
    void SetUp() override {
        System::IWorldModify::clearWorld();

        nlohmann::json j;
        j["entities"] = {};
        j["layers"]["fg"] = {1.0, 0.95, 0.9};
        System::IWorldModify::loadWorldFromJson(j);

        extra_data_.this_components = std::make_shared<ComponentStore>();
        extra_data_.frame_timer = 0;
        extra_data_.variables = std::make_shared<scripting::VariableMap>();
    }

    scripting::return_types parseAndRun(std::string source) {
        auto program = Program::loadProgram(source);
        return program.run(extra_data_);
    }

  protected:
    Program::ExtraInputData extra_data_;
};

TEST_F(InterpreterTestFixture, IntLiteral) {
    auto output = parseAndRun("4");

    EXPECT_EQ(std::get<int>(output), 4);
}

TEST_F(InterpreterTestFixture, BoolLiteral) {
    auto output = parseAndRun("true");

    EXPECT_EQ(std::get<bool>(output), true);

    output = parseAndRun("false");

    EXPECT_EQ(std::get<bool>(output), false);
}

TEST_F(InterpreterTestFixture, StringLiteral) {
    auto output = parseAndRun("'string_literal'");

    EXPECT_EQ(std::get<std::string>(output), "string_literal");
}

TEST_F(InterpreterTestFixture, FloatLiteral) {
    auto output = parseAndRun("4.3");

    EXPECT_EQ(std::get<double>(output), 4.3);
}

TEST_F(InterpreterTestFixture, GetSetVariables) {
    auto output = parseAndRun("set 'test_var' 4");
    EXPECT_EQ(std::get<int>(output), 0);

    output = parseAndRun("get 'test_var'");
    EXPECT_EQ(std::get<int>(output), 4);

    output = parseAndRun("set 'test_var2' 7");
    EXPECT_EQ(std::get<int>(output), 0);

    output = parseAndRun("get 'test_var2'");
    EXPECT_EQ(std::get<int>(output), 7);

    output = parseAndRun("get 'test_var'");
    EXPECT_EQ(std::get<int>(output), 4);
}

TEST_F(InterpreterTestFixture, IncDecVar) {
    auto output = parseAndRun("set 'test_var' 4");
    EXPECT_EQ(std::get<int>(output), 0);

    output = parseAndRun("get 'test_var'");
    EXPECT_EQ(std::get<int>(output), 4);

    output = parseAndRun("inc_var 'test_var'");
    output = parseAndRun("get 'test_var'");
    EXPECT_EQ(std::get<int>(output), 5);

    output = parseAndRun("dec_var 'test_var'");
    output = parseAndRun("dec_var 'test_var'");
    output = parseAndRun("get 'test_var'");
    EXPECT_EQ(std::get<int>(output), 3);
}

TEST_F(InterpreterTestFixture, FrameTimer) {
    auto output = parseAndRun("frame_timer 2");

    EXPECT_EQ(std::get<bool>(output), false);

    extra_data_.frame_timer++;
    output = parseAndRun("frame_timer 2");
    EXPECT_EQ(std::get<bool>(output), false);

    extra_data_.frame_timer++;
    output = parseAndRun("frame_timer 2");
    EXPECT_EQ(std::get<bool>(output), true);
}

TEST_F(InterpreterTestFixture, AddInt) {
    auto output = parseAndRun("add 0 3");
    EXPECT_EQ(std::get<int>(output), 3);

    output = parseAndRun("add 2 5");
    EXPECT_EQ(std::get<int>(output), 7);

    output = parseAndRun("add 5 2");
    EXPECT_EQ(std::get<int>(output), 7);

    output = parseAndRun("add 4 -2");
    EXPECT_EQ(std::get<int>(output), 2);

    output = parseAndRun("add 3 -5");
    EXPECT_EQ(std::get<int>(output), -2);

    output = parseAndRun("add (add 1 25) 17");
    EXPECT_EQ(std::get<int>(output), 43);
}

TEST_F(InterpreterTestFixture, SubInt) {
    auto output = parseAndRun("sub 3 0");
    EXPECT_EQ(std::get<int>(output), 3);

    output = parseAndRun("sub 3 3");
    EXPECT_EQ(std::get<int>(output), 0);

    output = parseAndRun("sub 4 1");
    EXPECT_EQ(std::get<int>(output), 3);

    output = parseAndRun("sub 1 4");
    EXPECT_EQ(std::get<int>(output), -3);

    output = parseAndRun("sub -3 2");
    EXPECT_EQ(std::get<int>(output), -5);

    output = parseAndRun("sub 6 -1");
    EXPECT_EQ(std::get<int>(output), 7);
}

TEST_F(InterpreterTestFixture, PlayerPosition) {
    auto output = parseAndRun("position_x player");
    EXPECT_EQ(std::get<int>(output), 0);

    output = parseAndRun("position_y player");
    EXPECT_EQ(std::get<int>(output), 0);

    System::IWorldModify::getPlayer().lock()->getComponent<Transform>()->setPosition(5, 2);

    output = parseAndRun("position_x player");
    EXPECT_EQ(std::get<int>(output), 5);

    output = parseAndRun("position_y player");
    EXPECT_EQ(std::get<int>(output), 2);
}

TEST_F(InterpreterTestFixture, ThisPosition) {
    extra_data_.this_components->setComponent<Transform>(std::make_shared<Transform>(extra_data_.this_components));
    extra_data_.this_components->getComponent<Transform>()->setPosition(3, 6);

    auto output = parseAndRun("position_x this");
    EXPECT_EQ(std::get<int>(output), 3);

    output = parseAndRun("position_y this");
    EXPECT_EQ(std::get<int>(output), 6);
}

TEST_F(InterpreterTestFixture, PenetrationDistance) {
    auto str = R"--({
        "Collision": {
            "height": 200,
            "type": "static",
            "width": 100
        },
        "Transform": {
            "pos_x": 30,
            "pos_y": 10
        }
    })--";

    auto entity = BaseEntity::createFromJson(nlohmann::json::parse(str));
    System::IWorldModify::addEntity(entity);

    auto str2 = R"--({
        "Entity": "test_enemy"
    })--";
    auto entity2 = BaseEntity::createFromJson(nlohmann::json::parse(str2));
    System::IWorldModify::addEntity(entity2);

    extra_data_.this_components = entity2->components_;
    extra_data_.this_components->getComponent<Transform>()->setPosition(0, 0);
    extra_data_.this_components->getComponent<Collision>()->update();

    auto output = parseAndRun("sensor 'test_sensor'");
    EXPECT_EQ(std::get<bool>(output), true);

    output = parseAndRun("penetration_distance 'test_sensor' top_edge");
    EXPECT_EQ(std::get<int>(output), 120);

    output = parseAndRun("penetration_distance 'test_sensor' bottom_edge");
    EXPECT_EQ(std::get<int>(output), 140);

    output = parseAndRun("penetration_distance 'test_sensor' left_edge");
    EXPECT_EQ(std::get<int>(output), 65);

    output = parseAndRun("penetration_distance 'test_sensor' right_edge");
    EXPECT_EQ(std::get<int>(output), 125);
}

TEST_F(InterpreterTestFixture, ThisCollides) {
    System::IWorldModify::getPlayer().lock()->getComponent<Transform>()->setPosition(500, 500);

    extra_data_.this_components->setComponent<Transform>(std::make_shared<Transform>(extra_data_.this_components));
    extra_data_.this_components->getComponent<Transform>()->setPosition(0, 0);
    extra_data_.this_components->setComponent<Collision>(std::make_shared<Collision>(extra_data_.this_components));
    extra_data_.this_components->getComponent<Collision>()->getCollideable()->setHitbox(10, 10);

    auto output = parseAndRun("collides player");

    EXPECT_EQ(std::get<bool>(output), false);

    System::IWorldModify::getPlayer().lock()->getComponent<Transform>()->setPosition(0, 0);

    output = parseAndRun("collides player");

    EXPECT_EQ(std::get<bool>(output), true);
}

TEST_F(InterpreterTestFixture, Flag) {
    auto output = parseAndRun("flag 'InterperTestFlag'");

    EXPECT_EQ(std::get<bool>(output), false);

    System::getEnvironment()->setFlag("InterperTestFlag");
    output = parseAndRun("flag 'InterperTestFlag'");
    EXPECT_EQ(std::get<bool>(output), true);
}

TEST_F(InterpreterTestFixture, AnimationLooped) {
    auto str = R"--({
        "Entity": "test_enemy"
    })--";

    auto entity = BaseEntity::createFromJson(nlohmann::json::parse(str));

    extra_data_.this_components = entity->components_;

    for (auto i = 0; i < 15; i++) {
        auto output = parseAndRun("animation_looped");
        EXPECT_EQ(std::get<bool>(output), false);
        extra_data_.this_components->getComponent<Animation>()->update();
    }

    auto output = parseAndRun("animation_looped");
    EXPECT_EQ(std::get<bool>(output), true);
}

TEST_F(InterpreterTestFixture, Sensor) {

    auto str = R"--({
        "Collision": {
            "height": 100,
            "type": "static",
            "width": 100
        },
        "Transform": {
            "pos_x": 100,
            "pos_y": 0
        }
    })--";

    auto entity = BaseEntity::createFromJson(nlohmann::json::parse(str));
    System::IWorldModify::addEntity(entity);

    str = R"--({
        "Entity": "test_enemy"
    })--";

    entity = BaseEntity::createFromJson(nlohmann::json::parse(str));

    extra_data_.this_components = entity->components_;

    auto output = parseAndRun("sensor 'test_sensor'");
    EXPECT_EQ(std::get<bool>(output), false);

    extra_data_.this_components->getComponent<Transform>()->setPosition(100, 0);
    extra_data_.this_components->getComponent<Collision>()->update();

    output = parseAndRun("sensor 'test_sensor'");
    EXPECT_EQ(std::get<bool>(output), true);
}

TEST_F(InterpreterTestFixture, Greater) {
    auto output = parseAndRun("grt 4 2");

    EXPECT_EQ(std::get<bool>(output), true);

    output = parseAndRun("grt 1 5");

    EXPECT_EQ(std::get<bool>(output), false);

    output = parseAndRun("grt 3 3");

    EXPECT_EQ(std::get<bool>(output), false);
}

TEST_F(InterpreterTestFixture, Less) {
    auto output = parseAndRun("lss 3 9");

    EXPECT_EQ(std::get<bool>(output), true);

    output = parseAndRun("lss 7 2");

    EXPECT_EQ(std::get<bool>(output), false);

    output = parseAndRun("lss 2 2");

    EXPECT_EQ(std::get<bool>(output), false);
}

TEST_F(InterpreterTestFixture, And) {
    auto output = parseAndRun("and true true");

    EXPECT_EQ(std::get<bool>(output), true);

    output = parseAndRun("and false true");

    EXPECT_EQ(std::get<bool>(output), false);
}

TEST_F(InterpreterTestFixture, Or) {
    auto output = parseAndRun("or false true");

    EXPECT_EQ(std::get<bool>(output), true);

    output = parseAndRun("or false false");

    EXPECT_EQ(std::get<bool>(output), false);
}

TEST_F(InterpreterTestFixture, IfThen) {
    auto output = parseAndRun("if (grt 3 4) then true");

    EXPECT_EQ(std::get<int>(output), 0);

    output = parseAndRun("if (grt 4 3) then true");

    EXPECT_EQ(std::get<bool>(output), true);
}

TEST_F(InterpreterTestFixture, Action) {
    extra_data_.this_components->setComponent<Actions>(std::make_shared<Actions>(extra_data_.this_components));

    EXPECT_FALSE(extra_data_.this_components->getComponent<Actions>()->getActionState(Actions::Action::AI1));

    parseAndRun("action ai1");

    EXPECT_TRUE(extra_data_.this_components->getComponent<Actions>()->getActionState(Actions::Action::AI1));
}

TEST_F(InterpreterTestFixture, AddShadersToLayer) {
    auto j = nlohmann::json::parse(R"--({
        "layer": 0,
        "shaders": [
            {
                "shader": "kawase.frag",
                "uniforms": [
                    {
                        "type": "constant_float",
                        "name": "distance",
                        "value": 0.0
                    },
                    {
                        "type": "window_size",
                        "name": "render_size"
                    }
                ]
            }
        ]
    })--");

    extra_data_.this_components->setComponent<Rendering>(std::make_shared<Rendering>(extra_data_.this_components));
    extra_data_.this_components->getComponent<Rendering>()->reloadFromJson(j);

    // For now only check parsing
    // TODO Figure out how to verify
    parseAndRun("add_shader_to_layer 0 3");
}

TEST_F(InterpreterTestFixture, AddCameraTrauma) {
    EXPECT_EQ(System::getCamera()->getTrauma(), 0.0);

    parseAndRun("add_camera_trauma 0.6");

    EXPECT_EQ(System::getCamera()->getTrauma(), 0.6);
}

TEST_F(InterpreterTestFixture, Move) {
    extra_data_.this_components->setComponent<Movement>(std::make_shared<Movement>(extra_data_.this_components));

    // Ensure speed is relative
    extra_data_.this_components->getComponent<Movement>()->setVelocity(2.0, 1.0);

    auto output = parseAndRun("move this 1.2 5.5");
    EXPECT_EQ(std::get<int>(output), 0);

    auto vel_x = extra_data_.this_components->getComponent<Movement>()->getVelX();
    auto vel_y = extra_data_.this_components->getComponent<Movement>()->getVelY();

    EXPECT_EQ(vel_x, 3.2);
    EXPECT_EQ(vel_y, 6.5);

    // Test facing left
    extra_data_.this_components->getComponent<Movement>()->setVelocity(0.0, 0.0);
    extra_data_.this_components->getComponent<Movement>()->setFacingRight(false);

    output = parseAndRun("move this 3.1 0.0");
    EXPECT_EQ(std::get<int>(output), 0);

    vel_x = extra_data_.this_components->getComponent<Movement>()->getVelX();
    vel_y = extra_data_.this_components->getComponent<Movement>()->getVelY();

    EXPECT_EQ(vel_x, -3.1);
    EXPECT_EQ(vel_y, 0.0);
}

TEST_F(InterpreterTestFixture, SetVelocity) {
    extra_data_.this_components->setComponent<Movement>(std::make_shared<Movement>(extra_data_.this_components));

    // Ensure velocity is absolute
    extra_data_.this_components->getComponent<Movement>()->setVelocity(1.0, -1.0);

    auto output = parseAndRun("set_velocity this 1.2 5.5");
    EXPECT_EQ(std::get<int>(output), 0);

    auto vel_x = extra_data_.this_components->getComponent<Movement>()->getVelX();
    auto vel_y = extra_data_.this_components->getComponent<Movement>()->getVelY();

    EXPECT_EQ(vel_x, 1.2);
    EXPECT_EQ(vel_y, 5.5);
}

TEST_F(InterpreterTestFixture, SetPosition) {
    extra_data_.this_components->setComponent<Transform>(std::make_shared<Transform>(extra_data_.this_components));

    extra_data_.this_components->getComponent<Transform>()->setPosition(1.0, -1.0);

    auto output = parseAndRun("set_position this 4 7");
    EXPECT_EQ(std::get<int>(output), 0);

    auto pos_x = extra_data_.this_components->getComponent<Transform>()->getX();
    auto pos_y = extra_data_.this_components->getComponent<Transform>()->getY();

    EXPECT_EQ(pos_x, 4);
    EXPECT_EQ(pos_y, 7);
}

TEST_F(InterpreterTestFixture, EnableDisableAction) {
    extra_data_.this_components->setComponent<Actions>(std::make_shared<Actions>(extra_data_.this_components));

    extra_data_.this_components->getComponent<Actions>()->addAction(Actions::Action::MOVE_LEFT);

    auto isEnabled = extra_data_.this_components->getComponent<Actions>()->getActionState(Actions::Action::MOVE_LEFT, true);
    EXPECT_TRUE(isEnabled);

    auto output = parseAndRun("disable_action move_left");
    EXPECT_EQ(std::get<int>(output), 0);

    isEnabled = extra_data_.this_components->getComponent<Actions>()->getActionState(Actions::Action::MOVE_LEFT, true);
    EXPECT_FALSE(isEnabled);

    output = parseAndRun("enable_action move_left");
    EXPECT_EQ(std::get<int>(output), 0);

    isEnabled = extra_data_.this_components->getComponent<Actions>()->getActionState(Actions::Action::MOVE_LEFT, true);
    EXPECT_TRUE(isEnabled);
}

TEST_F(InterpreterTestFixture, OnEnterExit) {
    nlohmann::json j;
    j["Entity"] = "on_enter_exit";
    auto entitiy = BaseEntity::createFromJson(j);

    auto script = entitiy->getComponent<Scripting>();
    auto move = entitiy->getComponent<Movement>();
    auto state = entitiy->getComponent<Stateful>();

    ASSERT_EQ(move->getVelX(), 0.0);
    ASSERT_EQ(move->getVelY(), 0.0);

    // On exit, should change speed
    state->incomingEvent(state_utils::Event::ACTION_AI1);

    EXPECT_EQ(move->getVelX(), 100.0);
    EXPECT_EQ(move->getVelY(), 30.0);

    // No en enter/exit, speed should not change
    state->incomingEvent(state_utils::Event::ACTION_AI2);

    EXPECT_EQ(move->getVelX(), 100.0);
    EXPECT_EQ(move->getVelY(), 30.0);

    // On enter, should change speed
    state->incomingEvent(state_utils::Event::ACTION_AI1);

    EXPECT_EQ(move->getVelX(), 120.0);
    EXPECT_EQ(move->getVelY(), -20.0);
}

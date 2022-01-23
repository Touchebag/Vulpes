#include <gtest/gtest.h>

#include "ai/interpreter.h"
#include "ai/program.h"
#include "components/rendering/rendering.h"
#include "system/system.h"

using ai::Bool;
using ai::Target;

class InterpreterTestFixture : public ::testing::Test {
  public:
    InterpreterTestFixture() {
        System::IWorldModify::clearWorld();

        nlohmann::json j;
        j["entities"] = {};
        System::IWorldModify::loadWorldFromJson(j);
    }

    void SetUp() override {
        extra_data_.this_components = std::make_shared<ComponentStore>();
        extra_data_.frame_timer = 0;
    }

    int parseAndRun(std::string source) {
        auto program = Program::loadProgram(source);
        return Interpreter::executeProgram(program, extra_data_);
    }

  protected:
    Interpreter::ExtraInputData extra_data_;
};

TEST_F(InterpreterTestFixture, IntLiteral) {
    auto output = parseAndRun("4");

    EXPECT_EQ(output, 4);
}

TEST_F(InterpreterTestFixture, BoolLiteral) {
    auto output = parseAndRun("true");

    EXPECT_EQ(output, Bool::TRUE);

    output = parseAndRun("false");

    EXPECT_EQ(output, Bool::FALSE);
}

TEST_F(InterpreterTestFixture, StringLiteral) {
    auto program = Program::loadProgram("'string_literal'");

    EXPECT_EQ(program.getString(0), "string_literal");
}

TEST_F(InterpreterTestFixture, FloatLiteral) {
    auto program = Program::loadProgram("4.3");

    EXPECT_EQ(program.getFloat(0), 4.3);
}

TEST_F(InterpreterTestFixture, FrameTimer) {
    auto output = parseAndRun("frame_timer 2");

    EXPECT_EQ(output, Bool::FALSE);

    extra_data_.frame_timer++;
    output = parseAndRun("frame_timer 2");
    EXPECT_EQ(output, Bool::FALSE);

    extra_data_.frame_timer++;
    output = parseAndRun("frame_timer 2");
    EXPECT_EQ(output, Bool::TRUE);
}

TEST_F(InterpreterTestFixture, PlayerPosition) {
    auto output = parseAndRun("position_x player");

    EXPECT_EQ(output, 0);

    System::IWorldModify::getPlayer().lock()->getComponent<Transform>()->setPosition(5, 2);

    output = parseAndRun("position_x player");

    EXPECT_EQ(output, 5);
}

TEST_F(InterpreterTestFixture, ThisPosition) {
    auto output = parseAndRun("position_x this");

    EXPECT_EQ(output, 0);

    extra_data_.this_components->setComponent<Transform>(std::make_shared<Transform>(extra_data_.this_components));
    extra_data_.this_components->getComponent<Transform>()->setPosition(3, 6);

    output = parseAndRun("position_x this");

    EXPECT_EQ(output, 3);
}

TEST_F(InterpreterTestFixture, ThisCollides) {
    System::IWorldModify::getPlayer().lock()->getComponent<Transform>()->setPosition(500, 500);

    extra_data_.this_components->setComponent<Transform>(std::make_shared<Transform>(extra_data_.this_components));
    extra_data_.this_components->getComponent<Transform>()->setPosition(0, 0);
    extra_data_.this_components->setComponent<Collision>(std::make_shared<Collision>(extra_data_.this_components));
    extra_data_.this_components->getComponent<Collision>()->getCollideable()->setHitbox(10, 10);

    auto output = parseAndRun("collides player");

    EXPECT_EQ(output, Bool::FALSE);

    System::IWorldModify::getPlayer().lock()->getComponent<Transform>()->setPosition(0, 0);

    output = parseAndRun("collides player");

    EXPECT_EQ(output, Bool::TRUE);
}

TEST_F(InterpreterTestFixture, Flag) {
    auto output = parseAndRun("flag 'InterperTestFlag'");

    EXPECT_EQ(output, Bool::FALSE);

    System::getEnvironment()->setFlag("InterperTestFlag");
    output = parseAndRun("flag 'InterperTestFlag'");
    EXPECT_EQ(output, Bool::TRUE);
}

TEST_F(InterpreterTestFixture, AnimationLooped) {
    auto output = parseAndRun("animation_looped");

    EXPECT_EQ(output, Bool::FALSE);

    auto str = R"--({
        "Entity": "test_enemy"
    })--";

    auto entity = BaseEntity::createFromJson(nlohmann::json::parse(str));

    extra_data_.this_components = entity->components_;

    for (auto i = 0; i < 15; i++) {
        output = parseAndRun("animation_looped");
        EXPECT_EQ(output, Bool::FALSE);
        extra_data_.this_components->getComponent<Animation>()->update();
    }

    output = parseAndRun("animation_looped");
    EXPECT_EQ(output, Bool::TRUE);
}

TEST_F(InterpreterTestFixture, Greater) {
    auto output = parseAndRun("grt 4 2");

    EXPECT_EQ(output, Bool::TRUE);

    output = parseAndRun("grt 1 5");

    EXPECT_EQ(output, Bool::FALSE);

    output = parseAndRun("grt 3 3");

    EXPECT_EQ(output, Bool::FALSE);
}

TEST_F(InterpreterTestFixture, Less) {
    auto output = parseAndRun("lss 3 9");

    EXPECT_EQ(output, Bool::TRUE);

    output = parseAndRun("lss 7 2");

    EXPECT_EQ(output, Bool::FALSE);

    output = parseAndRun("lss 2 2");

    EXPECT_EQ(output, Bool::FALSE);
}

TEST_F(InterpreterTestFixture, And) {
    auto output = parseAndRun("and true true");

    EXPECT_EQ(output, Bool::TRUE);

    output = parseAndRun("and false true");

    EXPECT_EQ(output, Bool::FALSE);
}

TEST_F(InterpreterTestFixture, Or) {
    auto output = parseAndRun("or false true");

    EXPECT_EQ(output, Bool::TRUE);

    output = parseAndRun("or false false");

    EXPECT_EQ(output, Bool::FALSE);
}

TEST_F(InterpreterTestFixture, IfThen) {
    auto output = parseAndRun("if (grt 3 4) then true");

    EXPECT_EQ(output, 0);

    output = parseAndRun("if (grt 4 3) then true");

    EXPECT_EQ(output, Bool::TRUE);
}

TEST_F(InterpreterTestFixture, Action) {
    extra_data_.this_components->setComponent<Actions>(std::make_shared<Actions>(extra_data_.this_components));

    EXPECT_FALSE(extra_data_.this_components->getComponent<Actions>()->getActionState(Actions::Action::AI1));

    parseAndRun("action_ai1");

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

TEST_F(InterpreterTestFixture, Move) {
    extra_data_.this_components->setComponent<Movement>(std::make_shared<Movement>(extra_data_.this_components));

    // Ensure speed is relative
    extra_data_.this_components->getComponent<Movement>()->setVelocity(0.0, 1.0);

    auto output = parseAndRun("move this 1.2 5.5");
    EXPECT_EQ(output, 0);

    auto vel_x = extra_data_.this_components->getComponent<Movement>()->getVelX();
    auto vel_y = extra_data_.this_components->getComponent<Movement>()->getVelY();

    EXPECT_EQ(vel_x, 1.2);
    EXPECT_EQ(vel_y, 6.5);

    // Test facing left
    extra_data_.this_components->getComponent<Movement>()->setVelocity(0.0, 0.0);
    extra_data_.this_components->getComponent<Movement>()->setFacingRight(false);

    output = parseAndRun("move this 3.1 0.0");
    EXPECT_EQ(output, 0);

    vel_x = extra_data_.this_components->getComponent<Movement>()->getVelX();
    vel_y = extra_data_.this_components->getComponent<Movement>()->getVelY();

    EXPECT_EQ(vel_x, -3.1);
    EXPECT_EQ(vel_y, 0.0);
}

TEST_F(InterpreterTestFixture, OnEnterExit) {
    nlohmann::json j;
    j["Entity"] = "on_enter_exit";
    auto entitiy = BaseEntity::createFromJson(j);

    auto ai = entitiy->getComponent<AI>();
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

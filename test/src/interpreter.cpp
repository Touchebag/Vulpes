#include <gtest/gtest.h>

#include "ai/interpreter.h"
#include "ai/ai_utils.h"
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
        auto lexer_output = ai_utils::tokenizeString(source);
        auto program = ai_utils::loadInstructions(lexer_output);
        return Interpreter::executeProgram(program.first, extra_data_);
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

TEST_F(InterpreterTestFixture, PlayerPosition) {
    auto output = parseAndRun("player position_x");

    EXPECT_EQ(output, 0);

    System::IWorldModify::getPlayer().lock()->getComponent<Transform>()->setPosition(5, 2);

    output = parseAndRun("player position_x");

    EXPECT_EQ(output, 5);
}

TEST_F(InterpreterTestFixture, ThisPosition) {
    auto output = parseAndRun("this position_x");

    EXPECT_EQ(output, 0);

    extra_data_.this_components->setComponent<Transform>(std::make_shared<Transform>(extra_data_.this_components));
    extra_data_.this_components->getComponent<Transform>()->setPosition(3, 6);

    output = parseAndRun("this position_x");

    EXPECT_EQ(output, 3);
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

TEST_F(InterpreterTestFixture, Flag) {
    auto output = parseAndRun("flag InterperTestFlag");

    EXPECT_EQ(output, Bool::FALSE);

    System::getEnvironment()->setFlag("InterperTestFlag");
    output = parseAndRun("flag InterperTestFlag");
    EXPECT_EQ(output, Bool::TRUE);
}

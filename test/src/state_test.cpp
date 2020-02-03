#include <gtest/gtest.h>

#include "utils/state_handler.h"

class StateTestFixture : public ::testing::Test {
  public:
    void SetUp() {
        state_handler_.loadFromJson(nlohmann::json::parse(state_json_));
    }

    std::string state_json_ = R"--(
{
    "main": {
        "next_states": [
            {"event": "start", "state": "main2"},
            {"event": "touching_floor", "state": "main4"}
        ],
        "frame_timer": 1
    },
    "main2": {
        "next_states": [
            {"event": "touching_floor", "state": "main3"}
        ],
        "frame_timer": 2
    },
    "main3": {
        "next_states": [
            {"event": "touching_wall", "state": "main2"}
        ],
        "frame_timer": 3
    },
    "main4": {
        "next_states": [
        ],
        "frame_timer": 4
    }
}
        )--";

    StateHandler<state_utils::Properties> state_handler_;
};

TEST_F(StateTestFixture, NormalOperation) {
    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 1);

    state_handler_.incomingEvent(state_utils::Event::START);

    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 2);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);

    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 3);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_WALL);

    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 2);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);

    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 3);
}

TEST_F(StateTestFixture, NoFollowingMatch) {
    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 1);

    state_handler_.incomingEvent(state_utils::Event::DASHING);

    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 1);

    state_handler_.incomingEvent(state_utils::Event::MOVING);
    state_handler_.incomingEvent(state_utils::Event::FRAME_TIMEOUT);

    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 1);
}

TEST_F(StateTestFixture, NoFollowingStates) {
    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 1);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);

    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 4);

    state_handler_.incomingEvent(state_utils::Event::MOVING);
    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);
    state_handler_.incomingEvent(state_utils::Event::START);

    ASSERT_EQ(state_handler_.getStateData().frame_timer_, 4);
}

TEST_F(StateTestFixture, ParseError) {
    std::string state_json_invalid = R"--(
{
    "main": {
        "next_states": [
            {"event": "invalid_event", "state": "main"}
        ],
        "data": 1
    }
}
        )--";

    try {
        state_handler_.loadFromJson(nlohmann::json::parse(state_json_invalid));
    } catch (std::invalid_argument& e) {
        // Should throw invalid_argument
        return;
    }

    // Let other exceptions propagate and fail test case
}

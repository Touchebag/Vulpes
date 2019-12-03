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
            {"event": 0, "state": "main2"},
            {"event": 1, "state": "main4"}
        ],
        "data": 1
    },
    "main2": {
        "next_states": [
            {"event": 1, "state": "main3"},
            {"event": 3, "state": "main"}
        ],
        "data": 2
    },
    "main3": {
        "next_states": [
            {"event": 2, "state": "main2"}
        ],
        "data": 3
    },
    "main4": {
        "next_states": [
        ],
        "data": 4
    }
}
        )--";

    StateHandler<int> state_handler_;
};

TEST_F(StateTestFixture, NormalOperation) {
    ASSERT_EQ(state_handler_.getStateData(), 1);

    state_handler_.incomingEvent(state_utils::Event::START);

    ASSERT_EQ(state_handler_.getStateData(), 2);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);

    ASSERT_EQ(state_handler_.getStateData(), 3);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_WALL);

    ASSERT_EQ(state_handler_.getStateData(), 2);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);

    ASSERT_EQ(state_handler_.getStateData(), 3);
}

TEST_F(StateTestFixture, NoFollowingMatch) {
    ASSERT_EQ(state_handler_.getStateData(), 1);

    state_handler_.incomingEvent(state_utils::Event::DASHING);

    ASSERT_EQ(state_handler_.getStateData(), 1);

    state_handler_.incomingEvent(state_utils::Event::MOVING);
    state_handler_.incomingEvent(state_utils::Event::FRAME_TIMEOUT);

    ASSERT_EQ(state_handler_.getStateData(), 1);
}

TEST_F(StateTestFixture, NoFollowingStates) {
    ASSERT_EQ(state_handler_.getStateData(), 1);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);

    ASSERT_EQ(state_handler_.getStateData(), 4);

    state_handler_.incomingEvent(state_utils::Event::MOVING);
    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);
    state_handler_.incomingEvent(state_utils::Event::START);

    ASSERT_EQ(state_handler_.getStateData(), 4);
}

#include <gtest/gtest.h>

#include "components/state/state_handler.h"
#include "components/component_store.h"

class StateTestFixture : public ::testing::Test {
  public:
    void SetUp() override {
        state_handler_.reloadFromJson(nlohmann::json::parse(state_json_), std::make_shared<ComponentStore>());
    }

    std::string state_json_ = R"--(
{
    "states": {
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
}
        )--";

    StateHandler<state_utils::EntityContent> state_handler_;
};

TEST_F(StateTestFixture, NormalOperation) {
    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 1);

    state_handler_.incomingEvent(state_utils::Event::START);

    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 2);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);

    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 3);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_WALL);

    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 2);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);

    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 3);
}

TEST_F(StateTestFixture, NoFollowingMatch) {
    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 1);

    state_handler_.incomingEvent(state_utils::Event::DASHING);

    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 1);

    state_handler_.incomingEvent(state_utils::Event::MOVING);
    state_handler_.incomingEvent(state_utils::Event::FRAME_TIMEOUT);

    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 1);
}

TEST_F(StateTestFixture, NoFollowingStates) {
    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 1);

    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);

    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 4);

    state_handler_.incomingEvent(state_utils::Event::MOVING);
    state_handler_.incomingEvent(state_utils::Event::TOUCHING_FLOOR);
    state_handler_.incomingEvent(state_utils::Event::START);

    ASSERT_EQ(state_handler_.getStateData().state_props.frame_timer_, 4);
}

TEST_F(StateTestFixture, ParseError) {
    std::string state_json_invalid = R"--(
{
    "states": {
        "main": {
            "next_states": [
                {"event": "invalid_event", "state": "main"}
            ],
            "data": 1
        }
    }
}
    )--";

    try {
        state_handler_.reloadFromJson(nlohmann::json::parse(state_json_invalid), std::make_shared<ComponentStore>());
    } catch (std::invalid_argument& e) {
        // Should throw invalid_argument
        return;
    }

    // Let other exceptions propagate and fail test case
}

TEST_F(StateTestFixture, TemplateLoad) {
    std::string template_json = R"--(
{
    "templates": {
        "test_templ": {
            "frame_timer": 5,
            "next_states": [
                {"event": "falling", "state": "main3"}
            ],
            "properties": {
                "movement_locked_x": false,
                "touching_ground": false,
                "can_jump": false
            },
            "animation": "template",
            "collideables": [
                {
                    "type": "sensor",
                    "name": "ledge_climb_top",
                    "width": 100,
                    "height": 200,
                    "offset": {
                        "x": 50,
                        "y": -200
                    }
                }
            ]
        },
        "test_templ2": {
            "properties": {
                "touching_ground": true,
                "movement_locked_x": false
            }
        }
    },
    "states": {
        "main": {
            "templates": [
                "test_templ",
                "test_templ2"
            ],
            "next_states": [
                {"event": "start", "state": "main2"},
                {"event": "touching_floor", "state": "main4"}
            ],
            "properties": {
                "movement_locked_x": true,
                "movement_locked_y": true,
                "can_dash": false
            },
            "animation": "override",
            "collideables": [
                {
                    "type": "sensor",
                    "name": "ledge_climb_bottom",
                    "width": 30,
                    "height": 150,
                    "offset": {
                        "x": 40,
                        "y": 0
                    }
                }
            ]
        }
    }
}
    )--";

    state_handler_.reloadFromJson(nlohmann::json::parse(template_json), std::make_shared<ComponentStore>());

    auto state_props = state_handler_.getStateData().state_props;

    // Should take from template
    ASSERT_EQ(state_props.frame_timer_, 5);

    ASSERT_EQ(state_props.collideables.size(), 2);

    auto physics_props = state_handler_.getStateData().physics_props;

    // Should override template
    ASSERT_EQ(state_handler_.getStateData().state_props.animation_name, "override");

    // Should merge but override duplicates
    EXPECT_EQ(physics_props.movement_locked_x_, true);
    EXPECT_EQ(physics_props.movement_locked_y_, true);
    EXPECT_EQ(physics_props.touching_ground_, true);
    EXPECT_EQ(physics_props.can_dash_, false);
    EXPECT_EQ(physics_props.can_jump_, false);
}

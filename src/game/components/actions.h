#pragma once

#include <unordered_map>
#include <json.hpp>

class Actions {
  public:
    enum class Action {
        UNKNOWN,
        MOVE_LEFT,
        MOVE_RIGHT,
        JUMP,
        DASH,
    };

    void update();

    bool getActionState(Action action, bool first_frame = false);

    void addAction(Action action);
    void removeAction(Action action);

    static Action fromString(const std::string& action);

    // Refreshes all inputs
    // This is needed due to the event driven nature of key inputs
    // They will be removed manually on keyup events
    void updateActions();

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

  private:
    // ACTIVE = action has been held since at least one frame
    // FIRST_FRAME = action was triggered this frame
    // NOT_ACTIVE = action is stale and will be removed if no ACTIVE input is triggered
    enum ActionState {
        ACTIVE,
        FIRST_FRAME,
        NOT_ACTIVE,
    };

    // Buttons currently pressed will be stored in this
    std::unordered_map<Actions::Action, ActionState> current_actions_;

};

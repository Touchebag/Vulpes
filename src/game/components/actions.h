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

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

  private:
    // Buttons currently pressed will be stored in this.
    // The boolean value will only be true for the first frame each button is pressed
    std::unordered_map<Actions::Action, bool> current_actions_;

};

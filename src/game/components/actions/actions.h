#pragma once

#include "components/component.h"

#include "components/death.h"
#include "components/stateful.h"
#include "utils/bimap.h"

#include <unordered_map>
#include <json.hpp>

class Actions : public Component {
  public:
    enum class Action {
        UNKNOWN,
        MOVE_LEFT,
        MOVE_RIGHT,
        JUMP,

        // Upgrades
        WALL_JUMP,
        DOUBLE_JUMP,
        DASH,
        ATTACK,
        AIR_DIVE,

        INTERACT,

        // Number of normal actions
        NUM_ACTIONS,

        // These actions are treated separately

        // AI events
        AI_EVENT_1,
        AI_EVENT_2,

        // Misc
        DIE,
    };

    Actions(std::weak_ptr<Death> death);

    void update() override;

    bool getActionState(Action action, bool first_frame = false);

    virtual void addAction(Action action);
    void removeAction(Action action);

    static Action fromString(const std::string& action);

    // Refreshes all inputs
    // This is needed due to the event driven nature of key inputs
    // They will be removed manually on keyup events
    void updateActions();

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    static std::shared_ptr<Actions> createFromJson(nlohmann::json j, std::weak_ptr<Death> death, std::weak_ptr<Collision> coll, std::weak_ptr<StatefulEntity> state);

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

    std::weak_ptr<Death> death_;
};

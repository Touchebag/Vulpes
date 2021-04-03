#pragma once

#include "components/component.h"

#include "components/death.h"
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
        AIR_DIVE_BOUNCE,

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

    Actions(std::weak_ptr<ComponentStore> components);

    void update() override;

    bool getActionState(Action action, bool first_frame = false);

    virtual void addAction(Action action);
    void removeAction(Action action);

    void enableAction(Action action, bool enable);

    static Action fromString(const std::string& action);

    static std::shared_ptr<Actions> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

  private:
    std::array<bool, static_cast<int>(Action::NUM_ACTIONS)> enabled_actions_;

    bool isActionEnabled(Action action);

    void setAllEnabled(bool enable);

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

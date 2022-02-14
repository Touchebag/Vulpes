#pragma once

#include "components/component.h"

#include "components/death.h"
#include "utils/bimap.h"

#include <unordered_map>

class Actions : public Component {
  public:
    enum class Action {
        UNKNOWN,

        #define GENERATE_ENUM(action, string) action,
        #include "actions_enum.h"
        #undef GENERATE_ENUM

        // Number of normal actions
        NUM_ACTIONS,

        // Misc
        DIE,
    };

    explicit Actions(std::weak_ptr<ComponentStore> components);

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

    struct ActionState {
        // ACTIVE = action has been held since at least one frame
        // NOT_ACTIVE = action is stale and will be removed if no ACTIVE input is triggered
        enum ActiveState {
            ACTIVE,
            NOT_ACTIVE,
        };

        ActiveState active;

        // Number of frames that action has been active
        unsigned int frame_count = 0;
    };

    // Buttons currently pressed will be stored in this
    std::unordered_map<Actions::Action, ActionState> current_actions_;
};

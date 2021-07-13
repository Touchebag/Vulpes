const Bimap<std::string, Actions::Action> string_action_map = {
    {"unknown", Actions::Action::UNKNOWN},

    #define GENERATE_ENUM(action, string) {string, Actions::Action::action},
    #include "actions_enum.h"
    #undef GENERATE_ENUM

    {"ai_event_1", Actions::Action::AI_EVENT_1},
    {"ai_event_2", Actions::Action::AI_EVENT_2},

    {"die", Actions::Action::DIE},
};

const std::unordered_map<int, Actions::Action> id_actions_map = {
    {0, Actions::Action::JUMP},
    {1, Actions::Action::DASH},
    {2, Actions::Action::ATTACK1},
};

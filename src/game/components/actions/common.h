const Bimap<std::string, Actions::Action> string_action_map = {
    {"unknown", Actions::Action::UNKNOWN},

    #define GENERATE_ENUM(action, string) {string, Actions::Action::action},
    #include "actions_enum.h"
    #undef GENERATE_ENUM

    {"die", Actions::Action::DIE},
};

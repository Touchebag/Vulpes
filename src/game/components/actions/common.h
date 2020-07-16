const Bimap<std::string, Actions::Action> string_action_map = {
    {"unknown", Actions::Action::UNKNOWN},
    {"move_left", Actions::Action::MOVE_LEFT},
    {"move_right", Actions::Action::MOVE_RIGHT},
    {"jump", Actions::Action::JUMP},
    {"dash", Actions::Action::DASH},
    {"attack", Actions::Action::ATTACK},

    {"ai_event_1", Actions::Action::AI_EVENT_1},
    {"ai_event_2", Actions::Action::AI_EVENT_2},

    {"die", Actions::Action::DIE},
};

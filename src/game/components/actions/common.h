const Bimap<std::string, Actions::Action> string_action_map = {
    {"unknown", Actions::Action::UNKNOWN},
    {"move_left", Actions::Action::MOVE_LEFT},
    {"move_right", Actions::Action::MOVE_RIGHT},
    {"jump", Actions::Action::JUMP},

    {"wall_jump", Actions::Action::WALL_JUMP},
    {"double_jump", Actions::Action::DOUBLE_JUMP},
    {"dash", Actions::Action::DASH},
    {"attack", Actions::Action::ATTACK},
    {"air_dive", Actions::Action::AIR_DIVE},

    {"interact", Actions::Action::INTERACT},

    {"ai_event_1", Actions::Action::AI_EVENT_1},
    {"ai_event_2", Actions::Action::AI_EVENT_2},

    {"die", Actions::Action::DIE},
};

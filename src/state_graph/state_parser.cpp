#include <sstream>

#include "state_parser.h"

std::string StateParser::parseState(nlohmann::json j) {
    std::stringstream str;
    int current_id = j["id"].get<int>();

    str << current_id << " [ label=\"{ " << j["name"].get<std::string>();

    for (auto it = j.begin(); it != j.end(); ++it) {
        auto key = it.key();

        // Some fields are handled separately
        if (key != "name" && key != "id" && key != "frame_names" && key != "next_states") {
            str << " | { " << key << " | " << it.value().dump() << " } ";
        }
    }
    str << "}\" ];" << std::endl;

    nlohmann::json next_states = j["next_states"];

    for (auto it : next_states) {
        str << current_id << " -> " << it["state"].get<int>() << ";" << std::endl;
    }

    return str.str();
}

#include <sstream>

#include "state_parser.h"

std::string StateParser::parseState(std::string key, nlohmann::json j) {
    std::stringstream str;
    std::string current_id = key;

    str << key <<" [ label=\"{ " << key;

    for (auto it = j.begin(); it != j.end(); ++it) {
        auto key = it.key();

        // Some fields are handled separately
        if (key != "frame_names" && key != "next_states") {
            str << " | { " << key << " | " << it.value().dump() << " } ";
        }
    }
    str << "}\" ];" << std::endl;

    nlohmann::json next_states = j["next_states"];

    for (auto it : next_states) {
        str << current_id << " -> " << it["state"].get<std::string>() << ";" << std::endl;
    }

    return str.str();
}

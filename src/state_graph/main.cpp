#include <iostream>
#include <fstream>

#include "state_parser.h"
#include "file.h"
#include "log.h"

#define GRAPH_NAME "state"

int main() {
    auto jsonObj = file::loadJson("assets/player_state.json");

    if (!jsonObj) {
        LOGE("Failed to load json");
    }

    nlohmann::json j = *jsonObj;

    std::ofstream of("state.dot");

    if (!of.good()) {
        LOGE("Failed to open ouput file\n");
        exit(1);
    }

    of << "digraph " << GRAPH_NAME << " {" << std::endl;
    of << "node [shape=record];" << std::endl;

    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string test = StateParser::parseState(it.key(), it.value());
        of << test;
    }

    of << "}" << std::endl;
}

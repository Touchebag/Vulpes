#include <sstream>

#include "state_parser.h"

std::string StateParser::parseState(nlohmann::json j) {
    std::stringstream str;

    str << j["id"].get<int>() << " [ label=" << j["name"].get<std::string>() << " ];";

    return str.str();
}

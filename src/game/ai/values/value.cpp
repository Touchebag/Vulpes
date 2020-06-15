#include "value.h"

#include "constant.h"
#include "distance.h"
#include "player_value.h"
#include "this.h"
#include "ai/ai_utils.h"

#include "utils/log.h"

namespace ai {
namespace condition {

std::unique_ptr<const Value> Value::createFromString(std::string str) {
    auto index = str.find(' ');

    std::string type = str.substr(0, index);
    std::vector<std::string> arguments = {};

    if (index != std::string::npos) {
        arguments = ai_utils::tokenizeString(str.substr(index + 1, str.size()));
    }

    if (type == "constant") {
        if (arguments.size() != 1) {
            throw std::invalid_argument("constant: incorrect number of arguments");
        }
        return std::make_unique<Constant>(stoi(arguments[0]));
    } else if (type == "player") {
        return std::make_unique<PlayerValue>(arguments);
    } else if (type == "this") {
        return std::make_unique<This>(arguments);
    } else if (type == "distance") {
        auto args = ai_utils::extractSubstrings(str.substr(index + 1, str.size()));
        if (args.size() != 2) {
            throw std::invalid_argument("distance: incorrect number of arguments");
        }
        return std::make_unique<Distance>(Value::createFromString(arguments[0]),
                                          Value::createFromString(arguments[1]));
    };

    throw std::invalid_argument("Value: invalid type: " + type);
}

} // condition
} // ai

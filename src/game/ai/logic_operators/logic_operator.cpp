#include "logic_operator.h"

#include "ai/values/value.h"

#include "true.h"
#include "false.h"
#include "greater.h"
#include "less.h"
#include "frame_timer.h"
#include "animation_loop.h"
#include "collides.h"
#include "ai/ai_utils.h"

#include "utils/log.h"

namespace ai {
namespace condition {

std::shared_ptr<const LogicalOperator> LogicalOperator::createFromString(const std::string& str) {
    auto index = str.find(' ');

    std::string operation = str.substr(0, index);
    std::vector<std::string> arguments = {};

    if (index != std::string::npos) {
        // index +1 to skip extra space
        arguments = ai_utils::extractSubstrings(str.substr(index + 1, str.size()));
    }

    if (operation == "true") {
        return std::make_shared<True>();
    } else if (operation == "false") {
        return std::make_shared<False>();
    } else if (operation == "grt") {
        if (arguments.size() != 2) {
            throw std::invalid_argument("grt: incorrect number of arguments");
        }
        return std::make_shared<Greater>(Value::createFromString(arguments[0]),
                                         Value::createFromString(arguments[1]));
    } else if (operation == "lss") {
        if (arguments.size() != 2) {
            throw std::invalid_argument("lss: incorrect number of arguments");
        }
        return std::make_shared<Less>(Value::createFromString(arguments[0]),
                                      Value::createFromString(arguments[1]));
    } else if (operation == "frame_timer") {
        if (arguments.size() != 1) {
            throw std::invalid_argument("frame_timer: incorrect number of arguments");
        }
        return std::make_shared<FrameTimer>(Value::createFromString(arguments[0]));
    } else if (operation == "animation_loop") {
        return std::make_shared<AnimationLoop>();
    } else if (operation == "collides") {
        return std::make_shared<Collides>(arguments[0]);
    };

    throw std::invalid_argument("LogicalOperator: invalid operation: " + operation);
}

std::shared_ptr<const LogicalOperator> LogicalOperator::createFromJson(nlohmann::json j) {
    return createFromString(j.get<std::string>());
}

} // condition
} // ai

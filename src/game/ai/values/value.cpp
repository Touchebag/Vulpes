#include "value.h"

#include "constant.h"
#include "distance.h"
#include "player_value.h"
#include "this.h"

#include "utils/log.h"

namespace ai {
namespace condition {

std::unique_ptr<const Value> Value::createFromJson(nlohmann::json j) {
    if (!j.contains("type")) {
        LOGD("%s\n", j.dump().c_str());
        throw std::invalid_argument("Value: type not found");
    }

    auto type = j["type"].get<std::string>();

    if (type == "constant") {
        if (j.contains("value")) {
            return std::make_unique<Constant>(j["value"]);
        } else {
            throw std::invalid_argument("Constant: value not found");
        }
    } else if (type == "player") {
        if (j.contains("value")) {
            return std::make_unique<PlayerValue>(j["value"]);
        } else {
            throw std::invalid_argument("Player: value not found");
        }
    } else if (type == "this") {
        if (j.contains("value")) {
            return std::make_unique<This>(j["value"]);
        } else {
            throw std::invalid_argument("This: value not found");
        }
    } else if (type == "distance") {
        if (j.contains("value")) {
            return std::make_unique<Distance>(j["value"]);
        } else {
            throw std::invalid_argument("Distance: values not found");
        }
    };


    throw std::invalid_argument("Value: invalid type: " + type);
}

} // condition
} // ai

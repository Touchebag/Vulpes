#include "logic_operator.h"

#include "ai/values/value.h"

#include "greater.h"
#include "less.h"
#include "frame_timer.h"
#include "animation_loop.h"

#include "utils/log.h"

namespace ai {
namespace condition {

std::shared_ptr<const LogicalOperator> LogicalOperator::createFromJson(nlohmann::json j) {
    if (!j.contains("type")) {
        LOGD("%s\n", j.dump().c_str());
        throw std::invalid_argument("LogicalOperator: type not found");
    }

    auto type = j["type"].get<std::string>();

    if (type == "grt") {
        if (j.contains("lhs") && j.contains("rhs")) {
            return std::make_shared<Greater>(Value::createFromJson(j["lhs"]),
                                             Value::createFromJson(j["rhs"]));
        } else {
            throw std::invalid_argument("Greater: rhs/lhs not found");
        }
    } else if (type == "lss") {
        if (j.contains("lhs") && j.contains("rhs")) {
            return std::make_shared<Less>(Value::createFromJson(j["lhs"]),
                                          Value::createFromJson(j["rhs"]));
        } else {
            throw std::invalid_argument("Less: rhs/lhs not found");
        }
    } else if (type == "frame_timer") {
        if (j.contains("timeout")) {
            return std::make_shared<FrameTimer>(Value::createFromJson(j["timeout"]));
        } else {
            throw std::invalid_argument("FrameTimer: timeout not found");
        }
    } else if (type == "animation_loop") {
        return std::make_shared<AnimationLoop>();
    };

    throw std::invalid_argument("LogicalOperator: invalid type: " + type);
}

} // condition
} // ai

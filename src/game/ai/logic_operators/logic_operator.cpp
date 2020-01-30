#include "logic_operator.h"

#include "ai/values/value.h"

#include "greater.h"
#include "less.h"

#include "utils/log.h"

namespace ai {
namespace condition {

std::unique_ptr<const LogicalOperator> LogicalOperator::loadFromJson(nlohmann::json j, std::weak_ptr<Transform> trans) {
    if (!j.contains("type")) {
        LOGD("%s\n", j.dump().c_str());
        throw std::invalid_argument("LogicalOperator: type not found");
    }

    auto type = j["type"].get<std::string>();

    if (type == "grt") {
        if (j.contains("lhs") && j.contains("rhs")) {
            return std::make_unique<Greater>(Value::loadFromJson(j["lhs"], trans),
                                             Value::loadFromJson(j["rhs"], trans));
        } else {
            throw std::invalid_argument("Greater: rhs/lhs not found");
        }
    } else if (type == "lss") {
        if (j.contains("lhs") && j.contains("rhs")) {
            return std::make_unique<Less>(Value::loadFromJson(j["lhs"], trans),
                                          Value::loadFromJson(j["rhs"], trans));
        } else {
            throw std::invalid_argument("Less: rhs/lhs not found");
        }
    };

    throw std::invalid_argument("LogicalOperator: invalid type: " + type);
}

} // condition
} // ai

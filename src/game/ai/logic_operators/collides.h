#pragma once

#include "logic_operator.h"
#include "ai/values/value.h"
#include "system/system.h"

namespace ai {
namespace condition {

class Collides : public LogicalOperator {
  public:
    Collides(std::string other_entity) {
        if (other_entity != "player") {
            LOGW("AI Collides: entity %s not supported, defaulting to player", other_entity.c_str());
        }
    }

    bool getValue(aiValues& values) const override {
        if (auto this_coll = values.coll.lock()) {
            if (auto player = System::IWorldRead::getPlayer().lock()) {
                return this_coll->collides(player->getComponent<Collision>());
            }
        }

        return false;
    }
};

} // condition
} // ai

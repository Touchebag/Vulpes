#pragma once

#include "logic_operator.h"
#include "ai/values/value.h"
#include "system/world.h"

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
            if (auto player = World::IWorldRead::getPlayer().lock()) {
                return this_coll->collides(player->components_->collision);
            }
        }

        return false;
    }
};

} // condition
} // ai

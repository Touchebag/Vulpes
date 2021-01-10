#include "collideable_transition.h"

#include "system/world.h"
#include "utils/log.h"

CollideableTransition::CollideableTransition(std::weak_ptr<Transform> trans) :
    Collideable(trans) {
}

void CollideableTransition::update() {
    auto player = World::IWorldRead::getPlayer().lock();

    if (player->collision_ && collides(player->collision_->getCollideable())) {
        World::IWorldModify::loadRoom(room_, entrance_id_);
    }
}

void CollideableTransition::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

    if (!j.contains("destination")) {
        throw std::invalid_argument("Transition: destination not found");
    }

    auto dest = j["destination"];

    if (!(dest.contains("room") && dest.contains("entrance_id"))) {
        throw std::invalid_argument("Transition: room/entrance_id not found");
    }

    room_ = dest["room"].get<std::string>();
    entrance_id_ = dest["entrance_id"].get<int>();
}

std::optional<nlohmann::json> CollideableTransition::outputToJson() {
    nlohmann::json base_json = Collideable::outputToJson().value();

    nlohmann::json j;
    j["room"] = room_;
    j["entrance_id"] = entrance_id_;
    base_json["destination"] = j;

    return base_json;
}

Collideable::CollisionType CollideableTransition::getType() const {
    return Collideable::CollisionType::TRANSITION;
}
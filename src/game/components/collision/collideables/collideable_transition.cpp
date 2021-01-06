#include "collideable_transition.h"

#include "system/world.h"
#include "utils/log.h"

CollisionTransition::CollisionTransition(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

void CollisionTransition::update() {
    auto player = World::IWorldRead::getPlayer().lock();

    if (collides(player->collision_)) {
        World::IWorldModify::loadRoom(room_, entrance_id_);
    }
}

void CollisionTransition::reloadFromJson(nlohmann::json j) {
    Collision::reloadFromJson(j);

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

std::optional<nlohmann::json> CollisionTransition::outputToJson() {
    nlohmann::json base_json = Collision::outputToJson().value();

    nlohmann::json j;
    j["room"] = room_;
    j["entrance_id"] = entrance_id_;
    base_json["destination"] = j;

    return base_json;
}

Collision::CollisionType CollisionTransition::getType() const {
    return Collision::CollisionType::TRANSITION;
}

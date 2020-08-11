#include "collision_interactable.h"

#include "system/world.h"

#include "utils/log.h"

CollisionInteractable::CollisionInteractable(std::weak_ptr<Transform> trans) :
    Collision(trans) {
}

void CollisionInteractable::reloadFromJson(nlohmann::json j) {
    Collision::reloadFromJson(j);

    if (j.contains("transition")) {
        auto transition_json = j["transition"];
        std::pair<std::string, int> transition;

        transition.first = transition_json["name"].get<std::string>();
        transition.second = transition_json["id"].get<int>();

        transition_ = {transition};
    }
}

std::optional<nlohmann::json> CollisionInteractable::outputToJson() {
    nlohmann::json j = Collision::outputToJson().value();

    if (transition_) {
        nlohmann::json transition_json;
        auto opt_val = transition_.value();

        transition_json["name"] = opt_val.first;
        transition_json["id"] = opt_val.second;

        j["transition"] = transition_json;
    }

    return j;
}

void CollisionInteractable::update() {
    if (World::IWorldRead::hasInteractTriggered()) {
        if (transition_) {
            World::IWorldModify::loadRoom(transition_.value().first, transition_.value().second);
        }
    }
}

Collision::CollisionType CollisionInteractable::getType() const {
    return Collision::CollisionType::INTERACTABLE;
}

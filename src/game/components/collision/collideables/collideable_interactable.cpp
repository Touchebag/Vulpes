#include "collideable_interactable.h"

#include "system/world.h"
#include "system/system.h"

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

    if (j.contains("cutscene")) {
        cutscene_ = j["cutscene"];
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

    if (cutscene_) {
        j["cutscene"] = cutscene_.value();
    }

    return j;
}

void CollisionInteractable::update() {
    if (World::IWorldRead::hasInteractTriggered()) {
        for (auto it : World::IWorldRead::getCollisions(Collision::CollisionType::PLAYER_HURTBOX)) {
            if (collides(it)) {
                if (transition_) {
                    World::IWorldModify::loadRoom(transition_.value().first, transition_.value().second);
                }
                if (cutscene_) {
                    System::getCutscene()->startCutscene(cutscene_.value());
                }
            }
        }
    }
}

Collision::CollisionType CollisionInteractable::getType() const {
    return Collision::CollisionType::INTERACTABLE;
}

#include "collideable_interactable.h"

#include "system/world.h"
#include "system/system.h"

#include "utils/log.h"

CollideableInteractable::CollideableInteractable(std::weak_ptr<Transform> trans) :
    Collideable(trans) {
}

void CollideableInteractable::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

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

std::optional<nlohmann::json> CollideableInteractable::outputToJson() {
    nlohmann::json j = Collideable::outputToJson().value();

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

void CollideableInteractable::update() {
    if (World::IWorldRead::hasInteractTriggered()) {
        for (auto it : World::IWorldRead::getCollisions(Collideable::CollisionType::PLAYER_HURTBOX)) {
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

Collideable::CollisionType CollideableInteractable::getType() const {
    return Collideable::CollisionType::INTERACTABLE;
}

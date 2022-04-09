#include "collideable_interactable.h"

#include "system/system.h"
#include "system/system.h"

#include "utils/log.h"

CollideableInteractable::CollideableInteractable(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
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
        cutscene_name_ = j["cutscene"];
        if (auto c_json = File().loadCutscene(cutscene_name_.value())) {
            cutscene_ = Cutscene::createFromJson(c_json.value());
        } else {
            LOGW("Failed to load cutscene %s", cutscene_name_.value().c_str());
        }
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
        j["cutscene"] = cutscene_name_.value();
    }

    return j;
}

void CollideableInteractable::update() {
    if (auto player = System::IWorldModify::getPlayer().lock()) {
        auto player_actions = player->getComponent<Actions>();

        // If player has not triggered interact, return early
        if (!player_actions || !player_actions->getActionState(Actions::Action::INTERACT, true)) {
            return;
        }

        // If we collide with player, execute interact
        if (auto player_coll = player->getComponent<Collision>()) {
            if (collides(player_coll->getCollideable())) {
                if (transition_) {
                    System::IWorldModify::loadRoom(transition_.value().first, transition_.value().second);
                }
                if (cutscene_) {
                    System::IWorldModify::setCutscene(cutscene_);
                }
            }
        }
    }
}

Collideable::CollisionType CollideableInteractable::getType() const {
    return Collideable::CollisionType::INTERACTABLE;
}

#include "collideable_hurtbox.h"

#include "components/component_store.h"

CollideableHurtbox::CollideableHurtbox(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
}

void CollideableHurtbox::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

    teams_.clear();

    if (j.contains("teams")) {
        for (int team : j["teams"]) {
            teams_.insert(team);
        }
    }
}

std::optional<nlohmann::json> CollideableHurtbox::outputToJson() {
    if (auto j = Collideable::outputToJson()) {
        auto j_val = j.value();

        if (!teams_.empty()) {
            j_val["teams"] = teams_;
        }

        return j_val;
    } else {
        return std::nullopt;
    }
}

Collideable::CollisionType CollideableHurtbox::getType() const {
    return Collideable::CollisionType::HURTBOX;
}

const std::set<int> CollideableHurtbox::getTeams() const {
    return teams_;
}

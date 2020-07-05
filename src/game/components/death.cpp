#include "death.h"

#include "base_entity.h"

void Death::update() {
}

void Death::reloadFromJson(nlohmann::json j) {
    if (j.contains("death_entity")) {
        death_entity_json_ = j["death_entity"];
    }
}

std::optional<nlohmann::json> Death::outputToJson() {
    nlohmann::json j;

    if (death_entity_json_) {
        j["death_entity"] = death_entity_json_.value();
    }

    return j;
}

std::optional<nlohmann::json> Death::getDeathEntityJson() {
    return death_entity_json_;
}

void Death::setDead() {
    is_dead_ = true;
}

bool Death::isDead() {
    return is_dead_;
}

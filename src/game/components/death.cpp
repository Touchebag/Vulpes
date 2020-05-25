#include "death.h"

void Death::update() {
}

void Death::reloadFromJson(nlohmann::json) {
}

std::optional<nlohmann::json> Death::outputToJson() {
    nlohmann::json j;
    return j;
}

void Death::setDead() {
    is_dead_ = true;
}

bool Death::isDead() {
    return is_dead_;
}

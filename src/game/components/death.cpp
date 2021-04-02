#include "death.h"

#include "base_entity.h"

Death::Death(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void Death::update() {
}

std::shared_ptr<Death> Death::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<Death>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void Death::reloadFromJson(nlohmann::json j, File /* file_instance */) {
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

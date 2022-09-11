#include "collideable_sensor.h"

#include "system/system.h"

#include "utils/log.h"

namespace {

bool sharesElements(const std::set<int>& first, const std::set<int>& second) {
    for (auto team : first) {
        // If they share a team do not take damage
        if (second.count(team) > 0) {
            return true;
        }
    }

    return false;
}

} // namespace

CollideableSensor::CollideableSensor(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
}

void CollideableSensor::update() {
    Collideable::update();

    for (auto it : System::IWorldRead::getCollideables(target_type_)) {
        if (auto coll = it.lock()) {
            if (collides(coll)) {
                triggered_ = !sharesElements(getTeams(), coll->getTeams());
                return;
            }
        }
    }

    triggered_ = false;
}

bool CollideableSensor::hasTriggered() {
    return triggered_;
}

std::string CollideableSensor::getName() {
    return name_;
}

void CollideableSensor::reloadFromJson(nlohmann::json j) {
    Collideable::reloadFromJson(j);

    if (j.contains("target")) {
        target_type_ = string_type_map.at(j["target"]);
    } else {
        throw std::invalid_argument("Sensor, missing target");
    }

    if (j.contains("name")) {
        name_ = j["name"];
    } else {
        throw std::invalid_argument("Sensor, missing name");
    }
}

std::optional<nlohmann::json> CollideableSensor::outputToJson() {
    return std::nullopt;
}

Collideable::CollisionType CollideableSensor::getType() const {
    return CollisionType::SENSOR;
}

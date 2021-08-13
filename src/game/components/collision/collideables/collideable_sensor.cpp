#include "collideable_sensor.h"

#include "system/system.h"

#include "utils/log.h"

CollideableSensor::CollideableSensor(std::weak_ptr<ComponentStore> components) :
    Collideable(components) {
}

void CollideableSensor::update() {
    Collideable::update();

    for (auto it : System::IWorldRead::getCollideables(CollisionType::STATIC)) {
        if (auto coll = it.lock()) {
            if (collides(coll)) {
                triggered_ = true;
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

    name_ = j["name"];
}

std::optional<nlohmann::json> CollideableSensor::outputToJson() {
    return std::nullopt;
}

Collideable::CollisionType CollideableSensor::getType() const {
    return CollisionType::SENSOR;
}

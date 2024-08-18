#include "collision.h"

#include "utils/log.h"

#include "collideables/collideable_transition.h"
#include "collideables/collideable_collectible.h"
#include "collideables/collideable_interactable.h"

#include "collideables/movement/collideable_static.h"
#include "collideables/movement/collideable_slope.h"

#include "collideables/collideable_sensor.h"

#include "system/system.h"

Collision::Collision(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void Collision::update() {
    int direction_multiplier = 1;

    if (auto move = getComponent<Movement>()) {
        direction_multiplier = move->isFacingRight() ? 1 : -1;
    }

    for (auto it : temp_colls_) {
        it->update();
        it->setDirectionMultiplier(direction_multiplier);
    }

    for (auto coll : collideables_) {
        coll->update();
        coll->setDirectionMultiplier(direction_multiplier);
    }
}

std::shared_ptr<Collision> Collision::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File /* file_instance */) {
    auto collision = std::make_shared<Collision>(components);

    collision->reloadFromJson(j);

    return collision;
}

void Collision::reloadFromJson(nlohmann::json j, File /* file_instance */) {
    collideables_.clear();
    for (auto c : j["collideables"]) {
        collideables_.push_back(Collideable::createFromJson(c, component_store_));
    }
}

std::optional<nlohmann::json> Collision::outputToJson() {
    if (collideables_.size() > 0) {
        std::vector<nlohmann::json> colls;
        auto c = collideables_.at(0)->outputToJson();

        colls.push_back(collideables_.at(0)->outputToJson().value());

        nlohmann::json j;
        j["collideables"] = colls;
        return j;

    }

    return std::nullopt;
}

bool Collision::collides(std::weak_ptr<const Collision> other_entity) {
    if (auto other_coll = other_entity.lock()) {
        for (auto coll : other_coll->getCollideables()) {
            if (collides(coll)) {
                return true;
            }
        }
    }

    return false;
}

bool Collision::collides(std::weak_ptr<const Collideable> other_entity) {
    for (auto coll : collideables_) {
        if (coll->collides(other_entity)) {
            return true;
        }
    }

    return false;
}

std::vector<std::shared_ptr<Collideable>> Collision::getCollideables() const {
    return collideables_;
}

bool Collision::isSensorTriggered(std::string sensor_name) {
    try {
        if (auto coll = sensor_colls_.at(sensor_name).lock()) {
            return coll->hasTriggered();
        }
    } catch (std::out_of_range &e) {
        // No such sensor active, do nothing
    }

    return false;
}

std::shared_ptr<CollideableSensor> Collision::getSensor(std::string sensor_name) {
    try {
        if (auto coll = sensor_colls_.at(sensor_name).lock()) {
            return coll;
        }
    } catch (std::out_of_range &e) {
        // If sensor does not exist, just return none
    }

    return nullptr;
}

void Collision::addTemporaryCollideable(nlohmann::json j) {
    for (nlohmann::json j_coll : j) {
        auto coll = Collideable::createFromJson(j_coll, component_store_);

        temp_colls_.push_back(coll);

        if (auto sens_coll = std::dynamic_pointer_cast<CollideableSensor>(coll)) {
            sensor_colls_.insert({sens_coll->getName(), sens_coll});
        }

        System::IWorldModify::addCollideable(coll);
    }
}

void Collision::clearTemporaryCollideables() {
    temp_colls_.clear();
    sensor_colls_.clear();
}

void Collision::setCollideable(nlohmann::json j) {
    auto coll = Collideable::createFromJson(j, component_store_);

    if (coll) {
        collideables_.clear();
        collideables_.push_back(coll);
    } else {
        // This should never happen as createFromJson will throw
        // Message here for debug reasons
        LOGE("Collision, failed to create collidable");
    }
}

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
            if (collides(coll) && !sharesElements(getTeams(), coll->getTeams())) {
                last_trigger_coll_ = it;
                return;
            }
        }
    }

    last_trigger_coll_.reset();
}

bool CollideableSensor::hasTriggered() {
    return static_cast<bool>(last_trigger_coll_.lock());
}

std::string CollideableSensor::getName() {
    return name_;
}

int CollideableSensor::getPenetrationDistance(scripting::CollideableProperty edge) {
    int dist = 0;

    if (auto it = last_trigger_coll_.lock()) {
        auto this_hbox = getHitbox();
        auto this_trans = components_.lock()->getComponent<Transform>();
        auto other_hbox = it->getHitbox();
        auto other_trans = it->getTransform().lock();

        // This normally should not happen (as you would not be able to collide in the first place)
        // but components could potentially be deactivated in between checks
        if (this_trans && other_trans) {
            switch (edge) {
                case scripting::CollideableProperty::TOP_EDGE:
                    {
                        auto this_bot = static_cast<int>(this_trans->getY()) + this_hbox->getBottom();
                        auto other_top = static_cast<int>(other_trans->getY()) + other_hbox->getTop();

                        dist = this_bot - other_top;
                        break;
                    }
                case scripting::CollideableProperty::BOTTOM_EDGE:
                    {
                        auto this_top = static_cast<int>(this_trans->getY()) + this_hbox->getTop();
                        auto other_bot = static_cast<int>(other_trans->getY()) + other_hbox->getBottom();

                        dist = other_bot - this_top;
                        break;
                    }
                case scripting::CollideableProperty::LEFT_EDGE:
                    {
                        auto this_right = static_cast<int>(this_trans->getX()) + this_hbox->getRight();
                        auto other_left = static_cast<int>(other_trans->getX()) + other_hbox->getLeft();

                        dist = this_right - other_left;
                        break;
                    }
                case scripting::CollideableProperty::RIGHT_EDGE:
                    {
                        auto this_left = static_cast<int>(this_trans->getX()) + this_hbox->getLeft();
                        auto other_right = static_cast<int>(other_trans->getX()) + other_hbox->getRight();

                        dist = other_right - this_left;
                        break;
                    }
            }
        }
    }

    return dist;
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

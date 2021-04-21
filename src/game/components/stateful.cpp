#include "stateful.h"

#include "utils/file.h"
#include "utils/common.h"
#include "json.hpp"
#include "base_entity.h"

StatefulEntity::StatefulEntity(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void StatefulEntity::update() {
    if (frame_counter_-- == 0) {
        incomingEvent(state_utils::Event::FRAME_TIMEOUT);
    }
}

std::shared_ptr<StatefulEntity> StatefulEntity::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<StatefulEntity>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void StatefulEntity::reloadFromJson(nlohmann::json /* j */, File file_instance) {
    loadStates(file_instance);
}

std::optional<nlohmann::json> StatefulEntity::outputToJson() {
    nlohmann::json j;

    return {j};
}

void StatefulEntity::loadStates(File file_instance) {
    auto states = file_instance.loadStates();

    // TODO Error handling
    if (states) {
        state_handler_.reloadFromJson(states.value());
    }
}

void StatefulEntity::resetState() {
    state_handler_.resetState();

    incomingEvent(state_utils::Event::START);
}

void StatefulEntity::incomingEvent(state_utils::Event event) {
    auto new_state = state_handler_.incomingEvent(event);

    if (auto ns = new_state.lock()) {
        auto state_props = ns->getData().state_props;

        frame_counter_ = state_props.frame_timer_;
        if (auto tmp = getComponent<AnimatedEntity>()) {
            tmp->setFrameList(state_props.animation_name);
        }

        if (!getEntity().empty()) {
            auto entity = std::make_shared<BaseEntity>();
            entity->reloadFromJson(getEntity());

            if (auto subent = getComponent<Subentity>()) {
                subent->addEntity(entity);
            }
        }

        if (auto actions = getComponent<Actions>()) {
            actions->enableAction(Actions::Action::INTERACT, state_props.can_interact);
        }

        if (auto coll = getComponent<Collision>()) {
            if (!state_props.collideable.empty()) {
                coll->addTemporaryCollideable(state_props.collideable);
            } else {
                coll->clearTemporaryCollideables();
            }
        }
    }
}

const state_utils::StateProperties& StatefulEntity::getStateProperties() {
    return state_handler_.getStateData().state_props;
}

const state_utils::PhysicsProperties& StatefulEntity::getPhysicsProperties() {
    return state_handler_.getStateData().physics_props;
}

const nlohmann::json& StatefulEntity::getEntity() {
    return state_handler_.getStateData().entity;
}

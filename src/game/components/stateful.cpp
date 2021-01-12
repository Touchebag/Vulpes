#include "stateful.h"

#include "utils/file.h"
#include "utils/common.h"
#include "json.hpp"
#include "base_entity.h"

#include "utils/log.h"

StatefulEntity::StatefulEntity(std::weak_ptr<AnimatedEntity> animatedEntity, std::weak_ptr<Subentity> subentity, std::weak_ptr<Actions> actions) :
    animatedEntity_(animatedEntity),
    subentity_(subentity),
    actions_(actions) {
}

void StatefulEntity::update() {
    if (frame_counter_-- == 0) {
        incomingEvent(state_utils::Event::FRAME_TIMEOUT);
    }
}

void StatefulEntity::reloadFromJson(nlohmann::json j) {
    reloadFromJson(j, File());
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
        frame_counter_ = ns->getData().state_props.frame_timer_;
        if (auto tmp = animatedEntity_.lock()) {
            tmp->setFrameList(ns->getData().state_props.animation_name);
        }

        if (!getEntity().empty()) {
            auto entity = std::make_shared<BaseEntity>();
            entity->reloadFromJson(getEntity());

            if (auto subent = subentity_.lock()) {
                subent->addEntity(entity);
            }
        }

        if (auto actions = actions_.lock()) {
            actions->enableAction(Actions::Action::INTERACT, ns->getData().state_props.can_interact);
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

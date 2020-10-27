#include "stateful.h"

#include "utils/file.h"
#include "utils/common.h"
#include "json.hpp"
#include "base_entity.h"

#include "utils/log.h"

StatefulEntity::StatefulEntity(std::weak_ptr<AnimatedEntity> animatedEntity, std::weak_ptr<Subentity> subentity) :
    animatedEntity_(animatedEntity),
    subentity_(subentity) {
}

void StatefulEntity::update() {
    if (frame_counter_-- == 0) {
        incomingEvent(state_utils::Event::FRAME_TIMEOUT);
    }
}

void StatefulEntity::reloadFromJson(nlohmann::json j) {
    std::string main_entity_name = util::COMMON_ASSET_DIR;
    if (j.contains(util::MAIN_ENTITY_NAME)) {
        main_entity_name = j[util::MAIN_ENTITY_NAME].get<std::string>();
    }

    loadStates(main_entity_name);
}

std::optional<nlohmann::json> StatefulEntity::outputToJson() {
    nlohmann::json j;

    return {j};
}

void StatefulEntity::loadStates(std::string entity_name) {
    auto j = File::loadStates(entity_name);

    // TODO Error handling
    if (j) {
        state_handler_.reloadFromJson(j.value());
    }
}

void StatefulEntity::incomingEvent(state_utils::Event event) {
    auto new_state = state_handler_.incomingEvent(event);

    if (auto ns = new_state.lock()) {
        frame_counter_ = ns->getData().state_props.frame_timer_;
        if (auto tmp = animatedEntity_.lock()) {
            tmp->setFrameList(ns->getData().state_props.frame_names_);
        }

        if (!getEntity().empty()) {
            auto entity = std::make_shared<BaseEntity>();
            entity->reloadFromJson(getEntity());

            if (auto subent = subentity_.lock()) {
                subent->addEntity(entity);
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

#include "stateful.h"

#include "utils/file.h"
#include "utils/common.h"
#include "json.hpp"
#include "base_entity.h"

Stateful::Stateful(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void Stateful::update() {
    if (frame_counter_-- == 0) {
        incomingEvent(state_utils::Event::FRAME_TIMEOUT);
    }
}

std::shared_ptr<Stateful> Stateful::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<Stateful>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void Stateful::reloadFromJson(nlohmann::json /* j */, File file_instance) {
    loadStates(file_instance);
}

std::optional<nlohmann::json> Stateful::outputToJson() {
    nlohmann::json j;

    return {j};
}

void Stateful::loadStates(File file_instance) {
    auto states = file_instance.loadStates();

    // TODO Error handling
    if (states) {
        state_handler_.reloadFromJson(states.value());
    }
}

void Stateful::resetState() {
    state_handler_.resetState();

    incomingEvent(state_utils::Event::START);
}

void Stateful::incomingEvent(state_utils::Event event) {
    auto new_state = state_handler_.incomingEvent(event);

    if (auto ns = new_state.lock()) {
        auto state_props = ns->getData().state_props;

        frame_counter_ = state_props.frame_timer_;
        if (auto tmp = getComponent<Animation>()) {
            tmp->setFrameList(state_props.animation_name);
        }

        if (!getEntity().empty()) {
            auto entity = std::make_shared<BaseEntity>();
            auto ent_json = getEntity();
            entity->reloadFromJson(ent_json);

            // Inherit parent direction
            if (auto move = getComponent<Movement>()) {
                if (auto other_move = entity->getComponent<Movement>()) {
                    other_move->setFacingRight(move->isFacingRight());

                    // Force reload to set initial direction
                    other_move->reloadFromJson(ent_json["Movable"]);
                }
            }

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

const state_utils::StateProperties& Stateful::getStateProperties() {
    return state_handler_.getStateData().state_props;
}

const state_utils::PhysicsProperties& Stateful::getPhysicsProperties() {
    return state_handler_.getStateData().physics_props;
}

const nlohmann::json& Stateful::getEntity() {
    return state_handler_.getStateData().entity;
}

#include "stateful.h"

#include "utils/file.h"
#include "utils/common.h"
#include "base_entity.h"

#include "utils/log.h"

namespace {

void executeProgramsInVector(std::vector<Program> programs, std::shared_ptr<AI> ai) {
    if (ai) {
        for (auto& prog : programs) {
            ai->executeProgram(prog);
        }
    } else {
        LOGW("Stateful: no AI");
    }
}

} // namespace

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

void Stateful::reloadFromJson(nlohmann::json j, File file_instance) {
    original_json_ = {nullptr};
    if (j.is_null()) {
        auto states = file_instance.loadStates();

        // TODO Error handling
        if (states) {
            state_handler_.reloadFromJson(states.value(), component_store_.lock());
        }
    } else {
        state_handler_.reloadFromJson(j, component_store_.lock());
        original_json_ = j;
    }

    // Load initial temporary collideables
    checkTemporaryCollideables(state_handler_.getStateData().state_props);
}

std::optional<nlohmann::json> Stateful::outputToJson() {
    return {original_json_};
}

void Stateful::checkTemporaryCollideables(const state_utils::StateProperties& state_props) {
    if (auto coll = getComponent<Collision>()) {
        coll->clearTemporaryCollideables();

        if (!state_props.collideables.empty()) {
            coll->addTemporaryCollideable(state_props.collideables);
        }
    }
}

void Stateful::resetState() {
    state_handler_.resetState();

    incomingEvent(state_utils::Event::START);
}

void Stateful::incomingEvent(state_utils::Event event) {
    auto progs = state_handler_.getStateData().ai_on_exit;
    auto new_state = state_handler_.incomingEvent(event);

    if (auto ns = new_state.lock()) {
        // AI actions on state switch
        auto ai = getComponent<AI>();
        executeProgramsInVector(progs, ai);

        progs = ns->getData().ai_on_enter;
        executeProgramsInVector(progs, ai);

        auto state_props = ns->getData().state_props;
        auto physics_constants = ns->getData().physics_constants;

        frame_counter_ = state_props.frame_timer_;
        if (auto tmp = getComponent<Animation>()) {
            tmp->setFrameList(state_props.animation_name);
        }

        if (!getEntity().empty()) {
            if (auto subent = getComponent<Subentity>()) {
                auto ent_json = getEntity();
                auto entity = BaseEntity::createFromJson(ent_json);

                subent->addEntity(entity);
            }
        }

        checkTemporaryCollideables(state_props);

        if (auto phys = getComponent<Physics>()) {
            if (physics_constants) {
                phys->setPhysicsConstants(physics_constants.value());
            } else {
                phys->setPhysicsConstants();
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

const std::vector<Program>& Stateful::getAI() {
    return state_handler_.getStateData().ai;
}

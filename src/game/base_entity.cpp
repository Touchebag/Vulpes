#include "base_entity.h"

#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"

#include <unistd.h>

// If the original json contains a value it should override what is
// loaded from the entity file
#define createComponentFromJson(component_type, component_name, component_variable, file)\
    if (entity_json.contains(component_name)) {\
        components_->component_variable = component_type::createFromJson(entity_json[component_name], components_, file);\
    } else {\
        components_->component_variable.reset();\
    }

// Only explicitly save if not read from entity file
#define saveComponentToJson(component_name, component)\
    if (components_->component && (!entity_file_exclusives_.contains(component_name))) {\
        if (auto opt = components_->component->outputToJson()) {\
            j[component_name] = opt.value();\
        }\
    }

#define update_component(component)\
    if (components_->component) {\
        components_->component->update();\
    }


std::shared_ptr<BaseEntity> BaseEntity::createFromJson(const nlohmann::json& j) {
    auto ret_ptr = std::make_shared<BaseEntity>();

    ret_ptr->reloadFromJson(j);

    return ret_ptr;
}

void BaseEntity::reloadFromJson(const nlohmann::json& j) {
    File file_instance;

    nlohmann::json entity_json;

    if (j.contains("Entity")) {
        auto entity_name = j["Entity"].get<std::string>();
        file_instance = File(entity_name);

        if (auto j_entity = file_instance.loadEntityFromFile()) {
            entity_json = j_entity.value();
        } else {
            throw std::invalid_argument("File not found");
        }

        // Store all components from file
        entity_file_exclusives_ = entity_json;
        entity_file_name_ = entity_name;
    } else {
        entity_file_name_.clear();
    }

    entity_file_exclusives_.clear();
    entity_file_exclusives_ = entity_json;
    // Overload entity file components with local components
    for (auto it = j.begin(); it != j.end(); ++it) {
        // Remove duplicate entries and only store file exclusive
        if (entity_file_exclusives_.contains(it.key())) {
            entity_file_exclusives_.erase(it.key());
        }

        entity_json[it.key()] = it.value();
    }

    createComponentFromJson(Transform, "Transform", transform, file_instance);

    createComponentFromJson(Death, "Death", death, file_instance);

    createComponentFromJson(Actions, "Actions", actions, file_instance);

    createComponentFromJson(Collision, "Collision", collision, file_instance);

    createComponentFromJson(MovableEntity, "Movable", movableEntity, file_instance);

    createComponentFromJson(RenderableEntity, "Renderable", renderableEntity, file_instance);

    createComponentFromJson(AnimatedEntity, "Animated", animatedEntity, file_instance);

    createComponentFromJson(Subentity, "Subentity", subentity, file_instance);

    createComponentFromJson(StatefulEntity, "Stateful", statefulEntity, file_instance);

    if (components_->statefulEntity) {
        components_->statefulEntity->incomingEvent(state_utils::Event::START);
    }

    createComponentFromJson(Physics, "Physics", physics, file_instance);

    createComponentFromJson(AI, "AI", ai, file_instance);

    createComponentFromJson(Damageable, "Damageable", damageable, file_instance);
}

std::optional<nlohmann::json> BaseEntity::outputToJson() {
    nlohmann::json j;

    if (!entity_file_name_.empty()) {
        j["Entity"] = entity_file_name_;
    }

    saveComponentToJson("Transform", transform);

    saveComponentToJson("Collision", collision);

    saveComponentToJson("Movable", movableEntity);

    saveComponentToJson("Renderable", renderableEntity);

    saveComponentToJson("Animated", animatedEntity);

    saveComponentToJson("Stateful", statefulEntity);

    saveComponentToJson("Actions", actions);

    saveComponentToJson("Physics", physics);

    saveComponentToJson("AI", ai);

    saveComponentToJson("Damageable", damageable);

    saveComponentToJson("Subentity", subentity);

    saveComponentToJson("Death", death);

    return {j};
}

void BaseEntity::update() {
    // Check before decreasing
    // If previous frame was the last one (i.e. ticked down to 0) then trigger event before this frame
    update_component(statefulEntity);

    update_component(ai);

    update_component(physics);

    update_component(actions);

    update_component(collision);

    update_component(animatedEntity);

    update_component(damageable);

    update_component(subentity);
}


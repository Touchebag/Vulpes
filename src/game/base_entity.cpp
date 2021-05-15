#include "base_entity.h"

#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"

#include <unistd.h>

// If the original json contains a value it should override what is
// loaded from the entity file
#define createComponentFromJson(component_type, component_name, file)\
    if (entity_json.contains(component_name)) {\
        setComponent<component_type>(component_type::createFromJson(entity_json[component_name], components_, file));\
    } else {\
        setComponent<component_type>({});\
    }

// Only explicitly save if not read from entity file
#define saveComponentToJson(component_name, component_type)\
    if (getComponent<component_type>() && (!entity_file_exclusives_.contains(component_name))) {\
        if (auto opt = getComponent<component_type>()->outputToJson()) {\
            j[component_name] = opt.value();\
        }\
    }

#define update_component(component_type)\
    if (auto component = getComponent<component_type>()) {\
        component->update();\
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

    createComponentFromJson(Transform, "Transform", file_instance);

    createComponentFromJson(Death, "Death", file_instance);

    createComponentFromJson(Actions, "Actions", file_instance);

    createComponentFromJson(Collision, "Collision", file_instance);

    createComponentFromJson(MovableEntity, "Movable", file_instance);

    createComponentFromJson(RenderableEntity, "Renderable", file_instance);

    createComponentFromJson(AnimatedEntity, "Animated", file_instance);

    createComponentFromJson(Subentity, "Subentity", file_instance);

    createComponentFromJson(StatefulEntity, "Stateful", file_instance);

    if (getComponent<StatefulEntity>()) {
        getComponent<StatefulEntity>()->incomingEvent(state_utils::Event::START);
    }

    createComponentFromJson(Physics, "Physics", file_instance);

    createComponentFromJson(AI, "AI", file_instance);

    createComponentFromJson(Damageable, "Damageable", file_instance);
}

std::optional<nlohmann::json> BaseEntity::outputToJson() {
    nlohmann::json j;

    if (!entity_file_name_.empty()) {
        j["Entity"] = entity_file_name_;
    }

    saveComponentToJson("Transform", Transform);

    saveComponentToJson("Collision", Collision);

    saveComponentToJson("Movable", MovableEntity);

    saveComponentToJson("Renderable", RenderableEntity);

    saveComponentToJson("Animated", AnimatedEntity);

    saveComponentToJson("Stateful", StatefulEntity);

    saveComponentToJson("Actions", Actions);

    saveComponentToJson("Physics", Physics);

    saveComponentToJson("AI", AI);

    saveComponentToJson("Damageable", Damageable);

    saveComponentToJson("Subentity", Subentity);

    saveComponentToJson("Death", Death);

    return {j};
}

template <typename T>
std::shared_ptr<T> BaseEntity::getComponent() const {
    return components_->getComponent<T>();
}

template <typename T>
void BaseEntity::setComponent(std::shared_ptr<T> component) {
    components_->setComponent<T>(component);
}

void BaseEntity::update() {
    // Check before decreasing
    // If previous frame was the last one (i.e. ticked down to 0) then trigger event before this frame
    update_component(StatefulEntity);

    update_component(AI);

    update_component(Physics);

    update_component(MovableEntity);

    update_component(Actions);

    update_component(Collision);

    update_component(AnimatedEntity);

    update_component(Damageable);

    update_component(Subentity);
}


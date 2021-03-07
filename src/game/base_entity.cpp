#include "base_entity.h"

#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"

#include <unistd.h>

// If the original json contains a value it should override what is
// loaded from the entity file
#define createComponentFromJson(component_name, component, ...)\
    if (entity_json.contains(component_name)) {\
        component = component->createFromJson(entity_json[component_name], __VA_ARGS__);\
    } else {\
        component.reset();\
    }

#define loadComponentFromJson(component_name, component, component_ptr)\
    if (entity_json.contains(component_name) && component_ptr) {\
        component = component_ptr;\
        component->reloadFromJson(entity_json[component_name]);\
    } else {\
        component.reset();\
    }

#define loadComponentWithFileFromJson(component_name, file, component, component_ptr)\
    if (entity_json.contains(component_name) && component_ptr) {\
        component = component_ptr;\
        component->reloadFromJson(entity_json[component_name], file);\
    } else {\
        component.reset();\
    }

// Only explicitly save if not read from entity file
#define saveComponentToJson(component_name, component)\
    if (component && (!entity_file_exclusives_.contains(component_name))) {\
        if (auto opt = component->outputToJson()) {\
            j[component_name] = opt.value();\
        }\
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

    loadComponentFromJson("Transform", trans_, std::make_shared<Transform>());

    loadComponentFromJson("Death", death_, std::make_shared<Death>());

    createComponentFromJson("Actions", actions_, death_);

    createComponentFromJson("Collision", collision_, trans_, actions_);

    loadComponentFromJson("Movable", movableEntity_, std::make_shared<MovableEntity>(trans_, collision_));

    loadComponentWithFileFromJson("Renderable", file_instance, renderableEntity_, std::make_shared<RenderableEntity>(trans_, movableEntity_));

    loadComponentWithFileFromJson("Animated", file_instance, animatedEntity_, std::make_shared<AnimatedEntity>(renderableEntity_));

    loadComponentFromJson("Subentity", subentity_, std::make_shared<Subentity>(trans_, movableEntity_));

    loadComponentWithFileFromJson("Stateful", file_instance, statefulEntity_, std::make_shared<StatefulEntity>(animatedEntity_, subentity_, actions_, collision_));

    if (statefulEntity_) {
        statefulEntity_->incomingEvent(state_utils::Event::START);
    }

    loadComponentFromJson("Physics", physics_, std::make_shared<Physics>(statefulEntity_, movableEntity_, animatedEntity_, actions_));

    loadComponentWithFileFromJson("AI", file_instance, ai_, std::make_shared<AI>(actions_, trans_, collision_, animatedEntity_));

    createComponentFromJson("Damageable", damageable_, collision_, death_, statefulEntity_, renderableEntity_, movableEntity_);
}

std::optional<nlohmann::json> BaseEntity::outputToJson() {
    nlohmann::json j;

    if (!entity_file_name_.empty()) {
        j["Entity"] = entity_file_name_;
    }

    saveComponentToJson("Transform", trans_);

    saveComponentToJson("Collision", collision_);

    saveComponentToJson("Movable", movableEntity_);

    saveComponentToJson("Renderable", renderableEntity_);

    saveComponentToJson("Animated", animatedEntity_);

    saveComponentToJson("Stateful", statefulEntity_);

    saveComponentToJson("Actions", actions_);

    saveComponentToJson("Physics", physics_);

    saveComponentToJson("AI", ai_);

    saveComponentToJson("Damageable", damageable_);

    saveComponentToJson("Subentity", subentity_);

    saveComponentToJson("Death", death_);

    return {j};
}

void BaseEntity::update() {
    // Check before decreasing
    // If previous frame was the last one (i.e. ticked down to 0) then trigger event before this frame
    if (statefulEntity_) {
        statefulEntity_->update();
    }

    if (ai_) {
        ai_->update();
    }

    if (physics_) {
        physics_->update();
    }

    if (actions_) {
        actions_->update();
    }

    if (collision_) {
        collision_->update();
    }

    if (animatedEntity_) {
        animatedEntity_->update();
    }

    if (damageable_) {
        damageable_->update();
    }

    if (subentity_) {
        subentity_->update();
    }
}


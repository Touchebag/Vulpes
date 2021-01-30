#include "base_entity.h"

#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"

#include <unistd.h>

// If the origanl json contains a value it should override what is
// loaded from the entity file
#define createComponentFromJson(component_name, component, ...)\
    if (j.contains(component_name)) {\
        component = component->createFromJson(j[component_name], __VA_ARGS__);\
    } else if (entity_json.contains(component_name)) {\
        component = component->createFromJson(entity_json[component_name], __VA_ARGS__);\
        file_loaded_components_.insert(component_name);\
    } else {\
        component.reset();\
    }

#define loadComponentFromJson(component_name, component, component_ptr)\
    if (j.contains(component_name) && component_ptr) {\
        component = component_ptr;\
        component->reloadFromJson(j[component_name]);\
    } else if (entity_json.contains(component_name) && component_ptr) {\
        component = component_ptr;\
        component->reloadFromJson(entity_json[component_name]);\
        file_loaded_components_.insert(component_name);\
    } else {\
        component.reset();\
    }

#define loadComponentWithFileFromJson(component_name, file, component, component_ptr)\
    if (j.contains(component_name) && component_ptr) {\
        component = component_ptr;\
        component->reloadFromJson(j[component_name], file);\
    } else if (entity_json.contains(component_name) && component_ptr) {\
        component = component_ptr;\
        component->reloadFromJson(entity_json[component_name], file);\
        file_loaded_components_.insert(component_name);\
    } else {\
        component.reset();\
    }

// Only explicitly save if not read from entity file
#define saveComponentToJson(component_name, component)\
    if (component && (file_loaded_components_.count(component_name) == 0)) {\
        if (auto opt = component->outputToJson()) {\
            j[component_name] = opt.value();\
        }\
    }


std::shared_ptr<BaseEntity> BaseEntity::createFromJson(nlohmann::json j) {
    auto ret_ptr = std::make_shared<BaseEntity>();

    ret_ptr->reloadFromJson(j);

    return ret_ptr;
}

void BaseEntity::reloadFromJson(nlohmann::json j) {
    // TODO Duplicate arguments in orignal json is overwritten by entity load
    // Should be reversed instead
    File file_instance;

    nlohmann::json entity_json;
    file_loaded_components_.clear();

    if (j.contains("Entity")) {
        auto entity_name = j["Entity"].get<std::string>();
        file_instance = File(entity_name);

        if (auto j_entity = file_instance.loadEntityFromFile()) {
            j.insert(j_entity.value().begin(), j_entity.value().end());
            entity_json = j_entity.value();
        } else {
            throw std::invalid_argument("File not found");
        }

        entity_file_ = entity_name;
    } else {
        entity_file_.clear();
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

    if (!entity_file_.empty()) {
        j["Entity"] = entity_file_;
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


#include "base_entity.h"

#include "utils/log.h"
#include "utils/file.h"
#include "utils/common.h"

#include <unistd.h>

#define Q(str) #str
#define QUOTE(str) Q(str)

// If the original json contains a value it should override what is
// loaded from the entity file
#define createComponentFromJson(component)\
    if (entity_json.contains(QUOTE(component))) {\
        setComponent<component>(component::createFromJson(entity_json[QUOTE(component)], components_));\
    } else {\
        setComponent<component>({});\
    }

// Only explicitly save if not read from entity file
#define saveComponentToJson(component)\
    if (getComponent<component>() && (!entity_file_exclusives_.contains(QUOTE(component)))) {\
        if (auto opt = getComponent<component>()->outputToJson()) {\
            j[QUOTE(component)] = opt.value();\
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
    nlohmann::json entity_json;

    // Only used if loading entity. Create placeholder in outer scope
    File::DirectoryScope dir_scope(std::filesystem::path(""));

    //----- Fetching -----

    if (j.contains("Entity")) {
        auto entity_name = j["Entity"].get<std::string>();

        auto entity_path = File::getEntityDir() / entity_name;
        dir_scope = std::move(File::pushDirectory(entity_path));

        if (auto j_entity = File::loadEntityFromFile()) {
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

    //----- Parsing -----

    if (entity_json.contains("tag")) {
        tag_ = entity_json["tag"].get<std::string>();
    } else {
        tag_.clear();
    }

    if (entity_json.contains("condition")) {
        condition_ = entity_json["condition"].get<std::string>();
    } else {
        condition_.clear();
    }

    createComponentFromJson(Transform);

    createComponentFromJson(Death);

    createComponentFromJson(Actions);

    createComponentFromJson(Collision);

    createComponentFromJson(Movement);

    createComponentFromJson(Rendering);

    createComponentFromJson(Animation);

    createComponentFromJson(Subentity);

    createComponentFromJson(Physics);

    createComponentFromJson(Scripting);

    createComponentFromJson(Damageable);

    // State reads physics constants, needs to be initialised after
    createComponentFromJson(Stateful);
    if (getComponent<Stateful>()) {
        getComponent<Stateful>()->incomingEvent(state_utils::Event::START);
    }
}

std::optional<nlohmann::json> BaseEntity::outputToJson() {
    nlohmann::json j;

    if (!entity_file_name_.empty()) {
        j["Entity"] = entity_file_name_;
    }

    if (!condition_.empty()) {
        j["condition"] = condition_;
    }

    saveComponentToJson(Transform);

    saveComponentToJson(Collision);

    saveComponentToJson(Movement);

    saveComponentToJson(Rendering);

    saveComponentToJson(Animation);

    saveComponentToJson(Stateful);

    saveComponentToJson(Actions);

    saveComponentToJson(Physics);

    saveComponentToJson(Scripting);

    saveComponentToJson(Damageable);

    saveComponentToJson(Subentity);

    saveComponentToJson(Death);

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
    update_component(Stateful);

    update_component(Scripting);

    update_component(Physics);

    update_component(Movement);

    update_component(Actions);

    update_component(Collision);

    update_component(Animation);

    update_component(Damageable);

    update_component(Subentity);

    update_component(Rendering);
}


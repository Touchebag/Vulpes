#include "base_entity.h"

#include "utils/log.h"
#include "utils/file.h"

#include <unistd.h>

namespace {

template <class T>
std::shared_ptr<T> loadComponentFromJson(nlohmann::json j,
        const std::string& component_name, std::shared_ptr<T> component) {
    // TODO Error handling
    if (j.contains(component_name) && component) {
        component->reloadFromJson(j[component_name]);
    } else {
        component.reset();
    }

    return component;
}

} // namespace


std::shared_ptr<BaseEntity> BaseEntity::createFromJson(nlohmann::json j) {
    auto ret_ptr = std::make_shared<BaseEntity>();

    ret_ptr->reloadFromJson(j);

    return ret_ptr;
}

void BaseEntity::reloadFromJson(nlohmann::json j) {
    if (j.contains("Entity")) {
        auto file_name = j["Entity"];
        if (auto j_entity = File::loadEntityFromFile(file_name)) {
            j.insert(j_entity.value().begin(), j_entity.value().end());
        } else {
            throw std::invalid_argument("File not found");
        }

        entity_file_ = file_name;
    } else {
        entity_file_.clear();
    }

    trans_ = loadComponentFromJson(j, "Transform", std::make_shared<Transform>());

    if (j.contains("Collision")) {
        collision_ = Collision::createFromJson(j["Collision"], trans_);
    } else {
        collision_.reset();
    }

    movableEntity_ = loadComponentFromJson(j, "Movable", std::make_shared<MovableEntity>(trans_, collision_));

    renderableEntity_ = loadComponentFromJson<RenderableEntity>(j, "Renderable", std::make_shared<RenderableEntity>(trans_, movableEntity_));

    animatedEntity_ = loadComponentFromJson(j, "Animated", std::make_shared<AnimatedEntity>(renderableEntity_));

    subentity_ = loadComponentFromJson(j, "Subentity", std::make_shared<Subentity>(trans_, movableEntity_));

    statefulEntity_ = loadComponentFromJson(j, "Stateful", std::make_shared<StatefulEntity>(animatedEntity_, subentity_));

    if (statefulEntity_) {
        statefulEntity_->incomingEvent(state_utils::Event::START);
    }

    death_ = loadComponentFromJson(j, "Death", std::make_shared<Death>());

    if (j.contains("Actions")) {
        actions_ = Actions::createFromJson(j["Actions"], death_, collision_);
    } else {
        actions_.reset();
    }

    physics_ = loadComponentFromJson(j, "Physics", std::make_shared<Physics>(statefulEntity_, movableEntity_, animatedEntity_, actions_, collision_));

    ai_ = loadComponentFromJson(j, "AI", std::make_shared<AI>(actions_, trans_, collision_, animatedEntity_));

    if (j.contains("Damageable")) {
        damageable_ = Damageable::createFromJson(j["Damageable"], collision_, death_, statefulEntity_, renderableEntity_, movableEntity_);
    } else {
        damageable_.reset();
    }
}

std::optional<nlohmann::json> BaseEntity::outputToJson() {
    nlohmann::json j;

    if (trans_) {
        if (auto opt = trans_->outputToJson()) {
            j["Transform"] = opt.value();
        }
    }

    if (!entity_file_.empty()) {
        j["Entity"] = entity_file_;
    } else {
        if (collision_) {
            if (auto opt = collision_->outputToJson()) {
                j["Collision"] = opt.value();
            }
        }

        if (movableEntity_) {
            if (auto opt = movableEntity_->outputToJson()) {
                j["Movable"] = opt.value();
            }
        }

        if (renderableEntity_) {
            if (auto opt = renderableEntity_->outputToJson()) {
                j["Renderable"] = opt.value();
            }
        }

        if (animatedEntity_) {
            if (auto opt = animatedEntity_->outputToJson()) {
                j["Animated"] = opt.value();
            }
        }

        if (statefulEntity_) {
            if (auto opt = statefulEntity_->outputToJson()) {
                j["Stateful"] = opt.value();
            }
        }

        if (actions_) {
            if (auto opt = actions_->outputToJson()) {
                j["Actions"] = opt.value();
            }
        }

        if (physics_) {
            if (auto opt = physics_->outputToJson()) {
                j["Physics"] = opt.value();
            }
        }

        if (ai_) {
            if (auto opt = ai_->outputToJson()) {
                j["AI"] = opt.value();
            }
        }

        if (damageable_) {
            if (auto opt = damageable_->outputToJson()) {
                j["Damageable"] = opt.value();
            }
        }

        if (subentity_) {
            if (auto opt = subentity_->outputToJson()) {
                j["Subentity"] = opt.value();
            }
        }

        if (death_) {
            if (auto opt = death_->outputToJson()) {
                j["Death"] = opt.value();
            }
        }
    }

    return {j};
}

void BaseEntity::update() {
    // Check before decreasing
    // If previous frame was the last one (i.e. ticked down to 0) then trigger event before this frame
    if (statefulEntity_) {
        statefulEntity_->update();
    }

    if (collision_) {
        collision_->update();
    }

    if (ai_) {
        ai_->update();
    }

    if (physics_) {
        physics_->update();
    }

    if (animatedEntity_) {
        animatedEntity_->update();
    }

    if (actions_) {
        actions_->update();
    }

    if (damageable_) {
        damageable_->update();
    }

    if (subentity_) {
        subentity_->update();
    }
}


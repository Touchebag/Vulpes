#include "base_entity.h"

#include "log.h"

#include <unistd.h>

namespace {

template <class T>
std::shared_ptr<T> loadComponentFromJson(nlohmann::json j,
        const std::string& component_name, std::shared_ptr<T> component) {
    // TODO Error handling
    if (j.contains(component_name) && component) {
        component->loadFromJson(j[component_name]);
    } else {
        component.reset();
    }

    return component;
}

}

void BaseEntity::setPosition(int abs_x, int abs_y) {
    if (trans_) {
        trans_->setPosition(abs_x, abs_y);
    }
}

util::Point BaseEntity::getPosition() {
    if (trans_) {
        return {trans_->getX(), trans_->getY()};
    }

    return {0, 0};
}

void BaseEntity::setHitbox(int width, int height) {
    if (hitbox_) {
        hitbox_->setHitbox(width, height);
    }

    if (renderableEntity_) {
        renderableEntity_->setTextureCoords(0, 0, width, height);
    }
}

// TODO Change level editor commands to store all components
// and remove this
const Hitbox BaseEntity::getHitbox() {
    return *hitbox_;
}

void BaseEntity::loadFromJson(nlohmann::json j) {
    if (j.contains("Hitbox")) {
        hitbox_ = std::make_shared<Hitbox>();
        hitbox_->loadFromJson(j["Hitbox"]);
    }

    if (j.contains("Transform")) {
        trans_ = std::make_shared<Transform>();
        trans_->loadFromJson(j["Transform"]);
    }

    collision_ = loadComponentFromJson<Collision>(j, "Collision", std::make_shared<Collision>(trans_, hitbox_));

    if (j.contains("Movable")) {
        movableEntity_ = std::make_shared<MovableEntity>(trans_, hitbox_, collision_);
        movableEntity_->loadFromJson(j["Movable"]);
    }

    renderableEntity_ = loadComponentFromJson<RenderableEntity>(j, "Renderable", std::make_shared<RenderableEntity>(trans_));
    // TODO Fix tiling
    if (hitbox_) {
        setHitbox(hitbox_->width_, hitbox_->height_);
    }

    if (j.contains("Animated")) {
        animatedEntity_ = std::make_shared<AnimatedEntity>(renderableEntity_);
        animatedEntity_->loadFromJson(j["Animated"]);
    }

    if (j.contains("Stateful")) {
        statefulEntity_ = std::make_shared<StatefulEntity>(animatedEntity_);
        statefulEntity_->loadFromJson(j["Stateful"]);
    }

    if (j.contains("Actions")) {
        actions_ = std::make_shared<Actions>();
        actions_->loadFromJson(j["Actions"]);
    }

    if (j.contains("Physics")) {
        physics_ = std::make_shared<Physics>(statefulEntity_, renderableEntity_, movableEntity_, animatedEntity_, actions_, collision_);
        physics_->loadFromJson(j["Physics"]);
    }
}

std::optional<nlohmann::json> BaseEntity::outputToJson() {
    nlohmann::json j;

    if (hitbox_) {
        if (auto opt = hitbox_->outputToJson()) {
            j["Hitbox"] = opt.value();
        }
    }

    if (trans_) {
        if (auto opt = trans_->outputToJson()) {
            j["Transform"] = opt.value();
        }
    }

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

    return {j};
}

void BaseEntity::update() {
    // Check before decreasing
    // If previous frame was the last one (i.e. ticked down to 0) then trigger event before this frame
    if (statefulEntity_) {
        statefulEntity_->update();
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
}


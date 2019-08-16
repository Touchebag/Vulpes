#include "base_entity.h"

#include "log.h"

#include <unistd.h>

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

// TODO Remove when change phusics to component
Hitbox BaseEntity::getAbsHitbox() {
    Hitbox abs_hitbox;

    if (hitbox_ && trans_) {
        abs_hitbox.setHitbox(hitbox_->width_, hitbox_->height_);
        abs_hitbox.setOffset({trans_->getX(), trans_->getY()});
    }

    return abs_hitbox;
}

void BaseEntity::loadFromJson(nlohmann::json j) {
    // TODO Error handling
    if (j.contains("Hitbox")) {
        hitbox_ = std::make_shared<Hitbox>();
        hitbox_->loadFromJson(j["Hitbox"]);
    }

    if (j.contains("Transform")) {
        trans_ = std::make_shared<Transform>();
        trans_->loadFromJson(j["Transform"]);
    }

    if (j.contains("Movable")) {
        movableEntity_ = std::make_shared<MovableEntity>(trans_, hitbox_);
        movableEntity_->loadFromJson(j["Movable"]);
    }

    if (j.contains("Renderable")) {
        renderableEntity_ = std::make_shared<RenderableEntity>(trans_);
        renderableEntity_->loadFromJson(j["Renderable"]);
        // TODO Fix
        if (hitbox_) {
            setHitbox(hitbox_->width_, hitbox_->height_);
        }
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

    return {j};
}

std::weak_ptr<RenderableEntity> BaseEntity::getRenderable() {
    return renderableEntity_;
}

void BaseEntity::update() {
}


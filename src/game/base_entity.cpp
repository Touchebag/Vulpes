#include "base_entity.h"

#include "log.h"

#include <unistd.h>

void BaseEntity::setPosition(int abs_x, int abs_y) {
    trans_->setPosition(abs_x, abs_y);
}

sf::Vector2i BaseEntity::getPosition() {
    return {trans_->getX(), trans_->getY()};
}

void BaseEntity::setHitbox(int width, int height) {
    hitbox_->setHitbox(width, height);

    if (renderableEntity_) {
        renderableEntity_->setTextureCoords(0, 0, width, height);
    }
}

const Hitbox BaseEntity::getHitbox() {
    return *hitbox_;
}

Hitbox BaseEntity::getAbsHitbox() {
    Hitbox abs_hitbox;
    abs_hitbox.setHitbox(hitbox_->width_, hitbox_->height_);
    abs_hitbox.setOffset({trans_->getX(), trans_->getY()});
    return abs_hitbox;
}

void BaseEntity::loadFromJson(nlohmann::json j) {
    // TODO Transform and Hitbox should be conditionally initialised here
    hitbox_ = std::make_shared<Hitbox>();
    trans_ = std::make_shared<Transform>();

    // TODO Error handling
    setPosition(j["position"]["x"].get<int>(),
                j["position"]["y"].get<int>());

    auto width = j["hitbox"]["width"].get<int>();
    auto height = j["hitbox"]["height"].get<int>();

    if (j.contains("Renderable")) {
        renderableEntity_ = std::make_shared<RenderableEntity>(trans_);
        renderableEntity_->loadFromJson(j["Renderable"]);
        setHitbox(width, height);
    }

    if (j.contains("Animated")) {
        animatedEntity_ = std::make_shared<AnimatedEntity>(renderableEntity_);
        animatedEntity_->loadFromJson(j["Animated"]);
    }
}

std::optional<nlohmann::json> BaseEntity::outputToJson() {
    nlohmann::json j;
    j["position"]["x"] = static_cast<int>(trans_->getX());
    j["position"]["y"] = static_cast<int>(trans_->getY());

    j["hitbox"]["width"] = static_cast<int>(hitbox_->width_);
    j["hitbox"]["height"] = static_cast<int>(hitbox_->height_);

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

    return {j};
}

std::weak_ptr<RenderableEntity> BaseEntity::getRenderable() {
    return renderableEntity_;
}

void BaseEntity::update() {
}


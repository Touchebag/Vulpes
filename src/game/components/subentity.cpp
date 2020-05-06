#include "subentity.h"

#include "system/world.h"

#include "utils/log.h"

Subentity::Subentity(std::weak_ptr<Transform> trans) :
    trans_(trans) {
}

void Subentity::addEntity(std::shared_ptr<BaseEntity> entity) {
    if (entity->trans_) {
        if (auto trans = trans_.lock()) {
            entity->trans_->setPosition(trans->getX(), trans->getY());
        }
    }

    // Add to world
    World::IWorldModify::addEntity(entity, World::Layer::MAIN);

    // Store handle locally
    entity_ = entity;

}

void Subentity::update() {
    if (auto entity = entity_.lock()) {
    }
}

void Subentity::reloadFromJson(nlohmann::json) {
}

std::optional<nlohmann::json> Subentity::outputToJson() {
    return {};
}

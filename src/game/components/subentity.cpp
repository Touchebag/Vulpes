#include "subentity.h"

#include "system/system.h"

#include "utils/log.h"

Subentity::Subentity(std::weak_ptr<ComponentStore> components) :
    Component(components) {
}

void Subentity::addEntity(std::shared_ptr<BaseEntity> entity) {
    // Add to world
    System::IWorldModify::addEntity(entity);

    // Store handle locally
    entity_ = entity;

    set_position();
}

void Subentity::update() {
}

void Subentity::set_position() {
    if (auto entity = entity_.lock()) {
        auto ent_trans = entity->getComponent<Transform>();
        auto ent_move = entity->getComponent<MovableEntity>();

        if (ent_trans && ent_move) {
            auto trans = getComponent<Transform>();
            auto movable = getComponent<MovableEntity>();

            if (trans && movable) {
                ent_trans->setPosition(trans->getX() + (ent_trans->getX() * (movable->isFacingRight() ? 1 : -1)) ,
                                       trans->getY() + ent_trans->getY());
                ent_move->setFacingRight(movable->isFacingRight());
            }
        }
    }
}

std::shared_ptr<Subentity> Subentity::createFromJson(nlohmann::json j, std::weak_ptr<ComponentStore> components, File file_instance) {
    auto ret_ptr = std::make_shared<Subentity>(components);

    ret_ptr->reloadFromJson(j, file_instance);

    return ret_ptr;
}

void Subentity::reloadFromJson(nlohmann::json /* j */, File /* file_instance */) {
}

std::optional<nlohmann::json> Subentity::outputToJson() {
    nlohmann::json j;
    return j;
}

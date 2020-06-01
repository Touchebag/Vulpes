#include "subentity.h"

#include "system/world.h"

#include "utils/log.h"

Subentity::Subentity(std::weak_ptr<Transform> trans, std::weak_ptr<MovableEntity> movable) :
    trans_(trans),
    movable_(movable) {
}

void Subentity::addEntity(std::shared_ptr<BaseEntity> entity) {
    // Add to world
    World::IWorldModify::addEntity(entity, World::Layer::MAIN);

    // Store handle locally
    entity_ = entity;

    set_position();
}

void Subentity::update() {
}

void Subentity::set_position() {
    if (auto entity = entity_.lock()) {
        auto ent_trans = entity->trans_;
        auto ent_move = entity->movableEntity_;

        if (ent_trans && ent_move) {
            auto trans = trans_.lock();
            auto movable = movable_.lock();

            if (trans && movable) {
                ent_trans->setPosition(trans->getX() + (ent_trans->getX() * (movable->facing_right_ ? 1 : -1)) ,
                                       trans->getY() + ent_trans->getY());
                ent_move->facing_right_ = movable->facing_right_;
            }
        }
    }
}

void Subentity::reloadFromJson(nlohmann::json) {
}

std::optional<nlohmann::json> Subentity::outputToJson() {
    nlohmann::json j;
    return j;
}

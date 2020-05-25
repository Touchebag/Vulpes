#include "subentity.h"

#include "system/world.h"

#include "utils/log.h"

Subentity::Subentity(std::weak_ptr<Transform> trans, std::weak_ptr<RenderableEntity> render) :
    trans_(trans),
    render_(render) {
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
        auto ent_rndr = entity->renderableEntity_;
        if (ent_trans && ent_rndr) {

            auto trans = trans_.lock();
            auto render = render_.lock();
            if (trans && render) {

                ent_trans->setPosition(trans->getX() + (ent_trans->getX() * (render->facing_right_ ? 1 : -1)) ,
                                       trans->getY() + ent_trans->getY());
                ent_rndr->facing_right_ = render->facing_right_;
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

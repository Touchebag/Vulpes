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
        auto this_move = getComponent<Movement>();
        auto sub_move = entity->getComponent<Movement>();
        int facing_right = 1;

        if (this_move && sub_move) {
            facing_right = this_move->isFacingRight() ? 1 : -1;
            sub_move->setFacingRight(this_move->isFacingRight());

            // Reload initial speed according to facing direction
            auto x = sub_move->getVelX();
            auto y = sub_move->getVelY();
            sub_move->setVelocity(x * facing_right, y);
        }

        auto this_trans = getComponent<Transform>();
        auto sub_trans = entity->getComponent<Transform>();

        if (this_trans && sub_trans) {
            sub_trans->setPosition(this_trans->getX() + (sub_trans->getX() * facing_right) ,
                                   this_trans->getY() +  sub_trans->getY());
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

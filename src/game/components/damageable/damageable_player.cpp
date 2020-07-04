#include "damageable_player.h"

#include "system/world.h"

DamageablePlayer::DamageablePlayer(std::weak_ptr<Collision> hurtbox,
                                   std::weak_ptr<Death> death,
                                   std::weak_ptr<StatefulEntity> state,
                                   std::weak_ptr<RenderableEntity> render,
                                   std::weak_ptr<MovableEntity> move) :
    Damageable(hurtbox, death, state, render, move) {
}

void DamageablePlayer::reloadFromJson(nlohmann::json j) {
    Damageable::reloadFromJson(j);
}

std::optional<nlohmann::json> DamageablePlayer::outputToJson() {
    auto j = Damageable::outputToJson();

    if (j) {
        j.value()["type"] = "player";
    }

    return j;
}

void DamageablePlayer::update() {
    Damageable::update();

    if (auto coll = hurtbox_.lock()) {
        for (auto& it : World::IWorldRead::getCollisions(Collision::CollisionType::HEALTH)) {
            if (auto other_coll = it.lock()) {
                if (coll->collides(other_coll)) {
                    health_ += other_coll->getHealth();
                }
            }
        }
    }
}

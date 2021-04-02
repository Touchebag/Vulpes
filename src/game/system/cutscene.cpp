#include "cutscene.h"

#include "utils/log.h"

void Cutscene::addPlayer(std::shared_ptr<Player> player) {
    EntityInformation temp_player;

    temp_player.entity = player;

    temp_player.actions = player->components_->actions;
    // TODO Replace with cutscene version
    player->components_->actions.reset();

    if (player->components_->movableEntity) {
        // Reset speed
        player->components_->movableEntity->move(0.0, 0.0);
    }

    temp_player.state = player->components_->statefulEntity;
    player->components_->statefulEntity.reset();

    temp_player.physics = player->components_->physics;
    player->components_->physics.reset();

    player_ = std::make_shared<EntityInformation>(temp_player);
}

void Cutscene::addEvent(unsigned int frame, CutsceneEvent event) {
    events_.push({frame, event});
}

unsigned int Cutscene::getNextEventFrame() {
    if (events_.empty()) {
        LOGW("Cutscene queue empty");

        return 0;
    } else {
        return events_.front().first;
    }
}

Cutscene::CutsceneEvent Cutscene::popEvent() {
    if (events_.empty()) {
        LOGW("Cutscene queue empty");

        return {};
    } else {
        auto event = events_.front().second;
        events_.pop();

        return event;
    }
}

std::shared_ptr<Cutscene::EntityInformation> Cutscene::getEntity(int entity_id) {
    if (entity_id == -1) {
        return player_;
    } else {
        // TODO Error handling
        return entities_.at(entity_id);
    }
}

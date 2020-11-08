#include "cutscene.h"

#include "utils/log.h"

void Cutscene::addPlayer(std::shared_ptr<Player> player) {
    EntityInformation temp_player;

    temp_player.entity = player;

    // TODO Action still listening to inputs
    // call setInactive?
    temp_player.actions = player->actions_;
    // TODO Replace with cutscene version
    player->actions_.reset();

    temp_player.state = player->statefulEntity_;
    player->statefulEntity_.reset();

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

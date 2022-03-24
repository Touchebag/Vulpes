#include "cutscene.h"

#include "system.h"

#include "utils/log.h"

namespace {

void resetEntity(std::shared_ptr<Cutscene::EntityInformation> entity_info) {
    auto entity = entity_info->entity;

    entity->setComponent<Actions>(entity_info->actions);
    entity->setComponent<Stateful>(entity_info->state);
    entity->setComponent<Physics>(entity_info->physics);

    if (entity->getComponent<Stateful>()) {
        entity->getComponent<Stateful>()->resetState();
    }
}

} // namespace

Cutscene::Cutscene() {
    frame_counter_ = 0;

    addPlayer(System::IWorldModify::getPlayer().lock());

    Cutscene::CutsceneEvent event;

    event.frames_remaining = 0;
    event.type = Cutscene::CutsceneEventType::ANIMATION;
    event.extra_data = "door_transition";
    event.entity_id = -1;
    addEvent(0, event);

    event.frames_remaining = 60;
    event.type = Cutscene::CutsceneEventType::END_CUTSCENE;
    addEvent(60, event);
}

Cutscene::~Cutscene() {
    stop();
}

std::shared_ptr<Cutscene> Cutscene::createFromJson(nlohmann::json /* j */) {
    return std::make_shared<Cutscene>();
}

bool Cutscene::isActive() {
    return !events_.empty();
}

void Cutscene::stop() {
    // TODO Restore all entities
    resetEntity(getEntity(-1));
}

void Cutscene::addPlayer(std::shared_ptr<Player> player) {
    EntityInformation temp_player;

    temp_player.entity = player;

    temp_player.actions = player->getComponent<Actions>();
    // TODO Replace with cutscene version
    player->setComponent<Actions>({});

    if (player->getComponent<Movement>()) {
        // Reset speed
        player->getComponent<Movement>()->move(0.0, 0.0);
    }

    temp_player.state = player->getComponent<Stateful>();
    player->setComponent<Stateful>({});

    temp_player.physics = player->getComponent<Physics>();
    player->setComponent<Physics>({});

    player_ = std::make_shared<EntityInformation>(temp_player);
}

void Cutscene::addEvent(int start_frame, CutsceneEvent event) {
    events_.push({start_frame, event});
}

void Cutscene::update() {
    if (events_.empty()) {
        return;
    }

    // TODO While loop to handle all events sharing frame
    if (frame_counter_ >= getNextEventFrame()) {
        execute_event(popEvent());
    }

    frame_counter_++;

    // If empty afterwards
    if (events_.empty()) {
        stop();
        return;
    }
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
        auto event = events_.front();
        events_.pop();

        return event.second;
    }
}

std::shared_ptr<Cutscene::EntityInformation> Cutscene::getEntity(int entity_id) {
    if (entity_id == -1) {
        return player_;
    } else {
        // TODO Error handling
        return entities_.at(static_cast<unsigned long long>(entity_id));
    }
}

void Cutscene::execute_event(const Cutscene::CutsceneEvent& event) {
    switch (event.type) {
        case Cutscene::CutsceneEventType::ANIMATION:
            {
                auto entity_info = getEntity(event.entity_id);
                // TODO Check variant type
                auto animation_name = std::get<std::string>(event.extra_data);

                entity_info->entity->getComponent<Animation>()->setFrameList(animation_name);
                break;
            }
        case Cutscene::CutsceneEventType::END_CUTSCENE:
            {
                // TODO Restore all entities
                resetEntity(getEntity(-1));
                break;
            }
        default:
            LOGD("Unknown event type");
            break;
    }
}

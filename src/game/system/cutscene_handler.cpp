#include "cutscene_handler.h"

#include "system/system.h"

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

void CutsceneHandler::startCutscene(std::string cutscene_file) {
    (void)cutscene_file;

    frame_counter_ = 0;

    std::shared_ptr<Cutscene> cutscene = std::make_shared<Cutscene>();

    cutscene->addPlayer(System::IWorldModify::getPlayer().lock());

    Cutscene::CutsceneEvent event;

    event.type = Cutscene::CutsceneEventType::ANIMATION;
    event.animation_name = "door_transition";
    event.entity_id = -1;
    cutscene->addEvent(0, event);

    event.type = Cutscene::CutsceneEventType::END_CUTSCENE;
    cutscene->addEvent(60, event);

    next_event_frame = cutscene->getNextEventFrame();

    current_cutscene_ = cutscene;
}

bool CutsceneHandler::isCutscenePlaying() {
    return static_cast<bool>(current_cutscene_);
}

void CutsceneHandler::update() {
    if (!current_cutscene_) {
        return;
    }

    // TODO While loop to handle all events sharing frame
    if (frame_counter_ >= next_event_frame) {
        execute_event(current_cutscene_->popEvent());
        if (current_cutscene_) {
            next_event_frame = current_cutscene_->getNextEventFrame();
        }
    }

    frame_counter_++;
}

void CutsceneHandler::execute_event(const Cutscene::CutsceneEvent& event) {
    switch (event.type) {
        case Cutscene::CutsceneEventType::ANIMATION:
            {
                auto entity_info = current_cutscene_->getEntity(event.entity_id);

                entity_info->entity->getComponent<Animation>()->setFrameList("door_transition");
                break;
            }
        case Cutscene::CutsceneEventType::END_CUTSCENE:
            {
                // TODO Restore all entities
                resetEntity(current_cutscene_->getEntity(-1));
                current_cutscene_.reset();
                break;
            }
        default:
            LOGD("Unknown event type");
            break;
    }
}

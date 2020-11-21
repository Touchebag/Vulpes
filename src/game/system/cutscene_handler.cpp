#include "cutscene_handler.h"

#include "system/world.h"

#include "utils/log.h"

namespace {

void resetEntity(std::shared_ptr<Cutscene::EntityInformation> entity_info) {
    auto entity = entity_info->entity;

    entity->actions_ = entity_info->actions;
    entity->statefulEntity_ = entity_info->state;
    entity->physics_ = entity_info->physics;

    if (entity->statefulEntity_) {
        entity->statefulEntity_->resetState();
    }
}

} // namespace

void CutsceneHandler::startCutscene(std::string cutscene_file) {
    frame_counter_ = 0;

    std::shared_ptr<Cutscene> cutscene = std::make_shared<Cutscene>();

    cutscene->addPlayer(World::IWorldModify::getPlayer().lock());

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

                entity_info->entity->animatedEntity_->setFrameList("door_transition");
                break;
            }
        case Cutscene::CutsceneEventType::END_CUTSCENE:
            {
                // TODO Restore all entities
                LOGD("END_CUTSCENE");
                resetEntity(current_cutscene_->getEntity(-1));
                current_cutscene_.reset();
                break;
            }
        default:
            LOGD("Unknown event type");
            break;
    }
}

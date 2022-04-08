#include "cutscene.h"

#include "system.h"
#include "utils/bimap.h"

#include "utils/log.h"

namespace {

const Bimap<std::string, Cutscene::CutsceneEventType> string_event_map = {
    {"animation", Cutscene::CutsceneEventType::ANIMATION}
};

void updateEntitiy(std::shared_ptr<BaseEntity> entity) {
    if (auto anim = entity->getComponent<Animation>()) {
        anim->update();
    }

    if (auto render = entity->getComponent<Rendering>()) {
        render->update();
    }
}

} // namespace

std::shared_ptr<Cutscene> Cutscene::createFromJson(nlohmann::json j) {
    auto cutscene = std::make_shared<Cutscene>();

    if (j.contains("events")) {
        for (auto it : j["events"]) {
            if (!it.contains("frame")) {
                throw std::invalid_argument("Cutscene: frame not found");
            }

            cutscene->addEvent(it["frame"], Cutscene::loadEventFromJson(it));
        }
    }

    return cutscene;
}

bool Cutscene::isActive() {
    return !(events_.empty() && active_events_.empty());
}

void Cutscene::start() {
    entities_ = System::IWorldModify::getEntitesByTags({"player"});
}

void Cutscene::addEvent(int start_frame, CutsceneEvent event) {
    events_.push({start_frame, event});
}

Cutscene::CutsceneEvent Cutscene::loadEventFromJson(nlohmann::json j) {
    CutsceneEvent event;

    try {
        auto type = string_event_map.at(j["event"].get<std::string>());

        event.type = type;
        event.entity_tag = j["entity"].get<std::string>();

        switch (type) {
            case CutsceneEventType::ANIMATION:
                event.extra_data = j["animation_name"].get<std::string>();
                break;
            default:
                LOGE("This should never happen");
                exit(EXIT_FAILURE);
        }
    } catch (std::out_of_range& e) {
        // TODO Error handling
        throw;
    } catch (nlohmann::json::parse_error& e) {
        // TODO Error handling
        throw;
    }

    return event;
}

void Cutscene::update() {
    if (events_.empty() && active_events_.empty()) {
        return;
    }

    while (frame_counter_ >= getNextEventFrame()) {
        active_events_.push_back(popEvent());
    }

    for (auto it = active_events_.begin(); it != active_events_.end();) {
        if (it->frames_remaining == 0) {
            it = active_events_.erase(it);
        } else {
            execute_event(*it);
            it->frames_remaining--;
            it++;
        }
    }

    for (auto entity : entities_) {
        updateEntitiy(entity.second);
    }

    frame_counter_++;
}

unsigned int Cutscene::getNextEventFrame() {
    if (events_.empty()) {
        return INT_MAX;
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

std::shared_ptr<BaseEntity> Cutscene::getEntity(std::string tag) {
    try {
        return entities_.at(tag);
    } catch (std::out_of_range& e) {
        LOGE("Cutscene: invalid tag %s", tag.c_str());
        throw;
    }
}

void Cutscene::execute_event(const Cutscene::CutsceneEvent& event) {
    switch (event.type) {
        case Cutscene::CutsceneEventType::ANIMATION:
            {
                auto entity = getEntity(event.entity_tag);
                // TODO Check variant type
                auto animation_name = std::get<std::string>(event.extra_data);

                entity->getComponent<Animation>()->setFrameList(animation_name);
                break;
            }
        default:
            LOGD("Unknown event type");
            break;
    }
}

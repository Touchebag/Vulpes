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

std::shared_ptr<Cutscene> Cutscene::loadCutscene(const std::string& cutscene_name) {
    auto cutscene = std::make_shared<Cutscene>();

    std::vector<std::pair<unsigned int, CutsceneEvent>> events;
    auto cutscene_dir_path = File::getCutsceneDir();

    File::pushDirectory(cutscene_dir_path / cutscene_name);

    // Load events
    if (auto j_opt = File::loadCutscene()) {
        auto j = j_opt.value();

        if (j.contains("events")) {
            for (auto it = j["events"].begin(); it != j["events"].end(); it++) {
                for (auto event : it.value()) {
                    events.push_back({stoi(it.key()), loadEventFromJson(event)});
                }
            }
        }

        std::sort(events.begin(), events.end(), [] (const auto& left, const auto& right) { return left.first < right.first; });
    } else {
        LOGE("Failed to load cutscene %s", cutscene_name.c_str());
        throw std::invalid_argument("");
    }

    // Load all cutscene specific entites
    if (std::filesystem::is_directory(File::getEntityDir())) {
        for (auto ent : File::getDirContents(File::getEntityDir())) {
            auto entity_name = ent.path().filename().string();

            nlohmann::json j;
            j["Entity"] = entity_name;

            cutscene->cutscene_entities_.insert_or_assign(entity_name, BaseEntity::createFromJson(j));
        }
    }

    cutscene->addEvents(events);

    File::popDirectory();

    return cutscene;
}

bool Cutscene::isActive() {
    return !(next_event_ == events_.end() && active_events_.empty());
}

void Cutscene::start() {
    frame_counter_ = 0;
    next_event_ = events_.begin();
    active_events_.clear();
    world_entities_ = System::IWorldModify::getEntitesByTags({"player"});
}

void Cutscene::addEvents(const std::vector<std::pair<unsigned int, CutsceneEvent>>& events) {
    events_ = events;
    next_event_ = events_.begin();
}

Cutscene::CutsceneEvent Cutscene::loadEventFromJson(nlohmann::json j) {
    CutsceneEvent event;

    try {
        auto type = string_event_map.at(j["event"].get<std::string>());

        event.type = type;
        event.entity_tag = j["entity"].get<std::string>();
        if (j.contains("active_frames")) {
            event.active_frames = j["active_frames"];
        }

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
    if (!isActive()) {
        return;
    }

    while (frame_counter_ >= getNextEventFrame()) {
        auto event = popEvent();
        active_events_.push_back({event.active_frames, event});
    }

    for (auto it = active_events_.begin(); it != active_events_.end();) {
        if (it->first == 0) {
            it = active_events_.erase(it);
        } else {
            executeEvent(it->second);
            it->first--;
            it++;
        }
    }

    for (auto entity : cutscene_entities_) {
        updateEntitiy(entity.second);
    }

    for (auto entity : world_entities_) {
        updateEntitiy(entity.second);
    }

    frame_counter_++;
}

unsigned int Cutscene::getNextEventFrame() {
    if (next_event_ >= events_.end()) {
        return INT_MAX;
    } else {
        return next_event_->first;
    }
}

Cutscene::CutsceneEvent Cutscene::popEvent() {
    if (next_event_ >= events_.end()) {
        LOGW("Cutscene queue empty");
        return {};
    } else {
        return (next_event_++)->second;
    }
}

std::shared_ptr<BaseEntity> Cutscene::getEntity(std::string tag) {
    try {
        if (cutscene_entities_.count(tag) != 0) {
            return cutscene_entities_.at(tag);
        } else {
            return world_entities_.at(tag);
        }
    } catch (std::out_of_range& e) {
        LOGE("Cutscene: invalid tag %s", tag.c_str());
        throw;
    }
}

void Cutscene::executeEvent(Cutscene::CutsceneEvent& event) {
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

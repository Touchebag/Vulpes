#include "cutscene.h"

#include "system.h"
#include "utils/bimap.h"

#include "utils/log.h"

namespace {

const Bimap<std::string, Cutscene::CutsceneEventType> string_event_map = {
    {"animation", Cutscene::CutsceneEventType::ANIMATION},
    {"fade_out", Cutscene::CutsceneEventType::FADE_OUT},
    {"fade_in", Cutscene::CutsceneEventType::FADE_IN},
    {"move", Cutscene::CutsceneEventType::MOVE},
};

void updateEntitiy(std::shared_ptr<BaseEntity> entity) {
    if (auto anim = entity->getComponent<Animation>()) {
        anim->update();
    }

    if (auto render = entity->getComponent<Rendering>()) {
        render->update();
    }

    if (auto move = entity->getComponent<Movement>()) {
        move->update();
    }
}

} // namespace

std::shared_ptr<Cutscene> Cutscene::loadCutscene(const std::string& cutscene_name) {
    auto cutscene = std::make_shared<Cutscene>();
    auto cutscene_dir_path = File::getCutsceneDir();

    File::pushDirectory(cutscene_dir_path / cutscene_name);

    // Load events
    if (auto j_opt = File::loadCutscene()) {
        auto j = j_opt.value();

        cutscene->reloadFromJson(j);
    } else {
        LOGE("Failed to load cutscene %s", cutscene_name.c_str());
        throw std::invalid_argument("");
    }

    auto entities_dir = File::getFullPath(File::getEntityDir());
    // Load all cutscene specific entites
    if (std::filesystem::is_directory(entities_dir)) {
        for (auto ent : File::getDirContents(File::getEntityDir())) {
            auto entity_name = ent.path().filename().string();

            nlohmann::json j;
            j["Entity"] = entity_name;
            auto entity = BaseEntity::createFromJson(j);

            cutscene->cutscene_entities_.insert_or_assign(entity->getTag(), entity);

            if (auto renderable = entity->getComponent<Rendering>()) {
                auto layer = static_cast<unsigned long long>(renderable->getLayer());

                if (layer >= cutscene->renderables_.size()) {
                    cutscene->renderables_.resize(layer + 1);
                }

                cutscene->renderables_.at(layer).push_back(renderable);
            }
        }
    }

    File::popDirectory();

    return cutscene;
}

void Cutscene::reloadFromJson(nlohmann::json j) {
    std::vector<std::pair<unsigned int, CutsceneEvent>> events;

    if (j.contains("events")) {
        for (auto it = j["events"].begin(); it != j["events"].end(); it++) {
            for (auto j_event : it.value()) {
                auto event = loadEventFromJson(j_event);
                auto frame = static_cast<unsigned int>(stoi(it.key()));

                events.push_back({frame, event});
                tags_.insert(event.entity_tag);

                // Update length
                total_length_ = std::max(frame + event.active_frames, total_length_);
            }
        }
    }

    if (j.contains("teardown")) {
        for (auto it = j["teardown"].begin(); it != j["teardown"].end(); it++) {
            teardown_events_.push_back(loadEventFromJson(*it));
        }
    }

    std::sort(events.begin(), events.end(), [] (const auto& left, const auto& right) { return left.first < right.first; });

    addEvents(events);
}

bool Cutscene::isActive() {
    return !(next_event_ == events_.end() && active_events_.empty());
}

void Cutscene::start() {
    frame_counter_ = 0;
    next_event_ = events_.begin();
    active_events_.clear();
    world_entities_ = System::IWorldModify::getEntitesByTags(tags_);
}

void Cutscene::addEvents(const std::vector<std::pair<unsigned int, CutsceneEvent>>& events) {
    events_ = events;
    next_event_ = events_.begin();
}

const std::vector<std::vector<std::weak_ptr<Rendering>>>& Cutscene::getRenderables() {
    return renderables_;
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
            case CutsceneEventType::FADE_OUT:
            case CutsceneEventType::FADE_IN:
                // Ceiling division magic
                event.extra_data = (255 + event.active_frames - 1) / event.active_frames;
                break;
            case CutsceneEventType::MOVE:
                event.extra_data = std::pair<float, float>{j["x_vel"].get<float>(), j["y_vel"].get<float>()};
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

    // If the cutscene became inactive after this frame we should run the
    // exit events
    if (!isActive()) {
        for (auto it = teardown_events_.begin(); it != teardown_events_.end(); it++) {
            executeEvent(*it);
        }
    }
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
    }

    return {};
}

void Cutscene::executeEvent(Cutscene::CutsceneEvent& event) {
    // TODO Check variant type
    switch (event.type) {
        case CutsceneEventType::ANIMATION:
            if (auto entity = getEntity(event.entity_tag)) {
                auto animation_name = std::get<std::string>(event.extra_data);

                entity->getComponent<Animation>()->setFrameList(animation_name);
            }

            break;
        case CutsceneEventType::FADE_OUT:
            if (auto entity = getEntity(event.entity_tag)) {
                auto fade_strength = static_cast<sf::Uint8>(std::get<unsigned int>(event.extra_data));

                auto render = entity->getComponent<Rendering>();
                auto current_color = render->getColor();

                // Avoid underflow
                fade_strength = std::min(fade_strength, current_color.a);

                current_color -= {0, 0, 0, fade_strength};

                render->setColor(current_color);
            }

            break;
        case CutsceneEventType::FADE_IN:
            if (auto entity = getEntity(event.entity_tag)) {
                auto fade_strength = static_cast<sf::Uint8>(std::get<unsigned int>(event.extra_data));

                auto render = entity->getComponent<Rendering>();
                auto current_color = render->getColor();

                if (255 - current_color.a) {
                    // Clamp if overflow
                    fade_strength = 255 - current_color.a;
                }

                current_color += {0, 0, 0, fade_strength};

                render->setColor(current_color);
            }

            break;
        case CutsceneEventType::MOVE:
            if (auto entity = getEntity(event.entity_tag)) {
                auto [x, y] = std::get<std::pair<float, float>>(event.extra_data);

                if (auto move = entity->getComponent<Movement>()) {
                    move->setVelocity(x, y);
                } else {
                    LOGW("Tag %s, missing move component", event.entity_tag.c_str());
                }
            }
            break;
        default:
            LOGD("Unknown event type");
            break;
    }
}

unsigned int Cutscene::getLength() {
    return total_length_;
}

unsigned int Cutscene::getCurrentFrame() {
    return frame_counter_;
}

void Cutscene::fastForward(unsigned int frames) {
    // Ensure you can't fast forward post the end of cutscene
    auto length = getLength();
    if (frame_counter_ + frames > length) {
        frames = length - frame_counter_;
    }

    while (frames-- > 0) {
        update();
    }
}

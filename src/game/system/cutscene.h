#pragma once

#include <queue>
#include <variant>
#include <set>

#include "base_entity.h"
#include "player.h"
#include "nlohmann/json.hpp"

class Cutscene {
  public:
    enum class CutsceneEventType {
        UNKNOWN,

        ANIMATION,
        FADE_OUT,
        FADE_IN,

        MOVE,
        SET_POSITION,

        SET_TEXTURE,
        ENABLE
    };

    struct CutsceneEvent {
        unsigned int active_frames = 1;

        CutsceneEventType type = CutsceneEventType::UNKNOWN;

        std::string entity_tag;

        std::vector<int> extra_data_int;
        std::vector<unsigned int> extra_data_uint;
        std::vector<float> extra_data_float;
        std::vector<std::string> extra_data_str;
    };

    static std::shared_ptr<Cutscene> loadCutscene(const std::string& cutscene_name);

    void update();
    void start();
    bool isActive();

    const std::vector<std::vector<std::weak_ptr<Rendering>>>& getRenderables();

    void reloadFromJson(nlohmann::json j);

    unsigned int getCurrentFrame();
    unsigned int getLength();
    void fastForward(unsigned int frames);

  private:
    static Cutscene::CutsceneEvent loadEventFromJson(nlohmann::json j);
    void addEvents(const std::vector<std::pair<unsigned int, CutsceneEvent>>& events);
    void executeEvent(const CutsceneEvent& event);

    void addRenderable(std::shared_ptr<Rendering> renderable, unsigned int layer);

    unsigned int getNextEventFrame();
    CutsceneEvent popEvent();

    void applyFunctionOnTaggedEntites(const std::string& tag, std::function<void(std::shared_ptr<BaseEntity>)>);

    std::shared_ptr<BaseEntity> getEntity(const std::string& tag);

    unsigned int frame_counter_ = 0;
    unsigned int total_length_ = 0;

    // Pair is starting frame
    std::vector<std::pair<unsigned int, CutsceneEvent>> events_ = {};
    std::vector<CutsceneEvent> teardown_events_ = {};

    std::vector<std::pair<unsigned int, CutsceneEvent>>::iterator next_event_ = events_.begin();
    std::vector<std::pair<int, CutsceneEvent>> active_events_;

    std::set<std::string> tags_;

    std::map<std::string, std::shared_ptr<BaseEntity>> world_entities_;
    std::map<std::string, std::shared_ptr<BaseEntity>> cutscene_entities_;
    std::vector<std::vector<std::weak_ptr<Rendering>>> renderables_;

    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures_;
};

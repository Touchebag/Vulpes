#pragma once

#include <queue>
#include <variant>

#include "base_entity.h"
#include "player.h"
#include "nlohmann/json.hpp"

class Cutscene {
  public:
    enum class CutsceneEventType {
        UNKNOWN,
        ANIMATION,
    };

    struct CutsceneEvent {
        int active_frames = 1;

        CutsceneEventType type = CutsceneEventType::UNKNOWN;

        std::string entity_tag;

        std::variant<std::string> extra_data;
    };

    static std::shared_ptr<Cutscene> loadCutscene(const std::string& cutscene_name);

    void update();
    void start();
    bool isActive();

  private:
    static Cutscene::CutsceneEvent loadEventFromJson(nlohmann::json j);
    void addEvents(const std::vector<std::pair<unsigned int, CutsceneEvent>>& events);
    void executeEvent(CutsceneEvent& event);

    unsigned int getNextEventFrame();
    CutsceneEvent popEvent();

    std::shared_ptr<BaseEntity> getEntity(std::string tag);

    unsigned int frame_counter_ = 0;

    // Pair is starting frame
    std::vector<std::pair<unsigned int, CutsceneEvent>> events_ = {};
    std::vector<std::pair<unsigned int, CutsceneEvent>>::iterator next_event_ = events_.begin();
    std::vector<std::pair<int, CutsceneEvent>> active_events_;

    std::map<std::string, std::shared_ptr<BaseEntity>> world_entities_;
    std::map<std::string, std::shared_ptr<BaseEntity>> cutscene_entities_;
};

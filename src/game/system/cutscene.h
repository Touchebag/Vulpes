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
        int frames_remaining = 1;

        CutsceneEventType type = CutsceneEventType::UNKNOWN;

        std::string entity_tag;

        std::variant<std::string> extra_data;
    };

    static std::shared_ptr<Cutscene> createFromJson(nlohmann::json);

    void update();
    void start();
    bool isActive();

  private:

    static Cutscene::CutsceneEvent loadEventFromJson(nlohmann::json j);
    void addEvent(int start_frame, CutsceneEvent event);
    void execute_event(const CutsceneEvent& event);

    unsigned int getNextEventFrame();
    CutsceneEvent popEvent();

    std::shared_ptr<BaseEntity> getEntity(std::string tag);

    unsigned int frame_counter_ = 0;

    // Pair is starting frame
    std::queue<std::pair<unsigned int, CutsceneEvent>> events_;
    std::vector<CutsceneEvent> active_events_;

    std::map<std::string, std::shared_ptr<BaseEntity>> entities_;
};

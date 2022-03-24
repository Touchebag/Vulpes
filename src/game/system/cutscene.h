#pragma once

#include <queue>
#include <variant>

#include "base_entity.h"
#include "player.h"
#include "nlohmann/json.hpp"

class Cutscene {
  public:
    struct EntityInformation {
        std::shared_ptr<BaseEntity> entity;
        std::shared_ptr<Actions> actions;
        std::shared_ptr<Stateful> state;
        std::shared_ptr<Physics> physics;
    };

    enum class CutsceneEventType {
        UNKNOWN,
        ANIMATION,

        END_CUTSCENE,
    };

    struct CutsceneEvent {
        int frames_remaining = 0;

        CutsceneEventType type = CutsceneEventType::UNKNOWN;

        // -1 to be used by player
        int entity_id = -2;

        std::variant<std::string> extra_data;
    };

    Cutscene();
    ~Cutscene();

    static std::shared_ptr<Cutscene> createFromJson(nlohmann::json);

    void update();

    void stop();

    bool isActive();

    void addPlayer(std::shared_ptr<Player> player);
    void addEvent(int start_frame, CutsceneEvent event);

    unsigned int getNextEventFrame();
    CutsceneEvent popEvent();

    std::shared_ptr<EntityInformation> getEntity(int entity_id);

  private:
    void execute_event(const CutsceneEvent& event);

    unsigned int frame_counter_ = 0;

    std::shared_ptr<EntityInformation> player_;
    std::vector<std::shared_ptr<EntityInformation>> entities_;

    // Pair is starting frame
    std::queue<std::pair<unsigned int, CutsceneEvent>> events_;
};

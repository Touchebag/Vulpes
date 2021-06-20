#pragma once

#include <queue>

#include "base_entity.h"
#include "player.h"

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
        CutsceneEventType type = CutsceneEventType::UNKNOWN;
        // -1 to be used by player
        int entity_id = -2;

        std::string animation_name;
    };

    void addPlayer(std::shared_ptr<Player> player);

    void addEvent(unsigned int frame, CutsceneEvent event);

    unsigned int getNextEventFrame();
    CutsceneEvent popEvent();

    std::shared_ptr<EntityInformation> getEntity(int entity_id);

  private:
    std::shared_ptr<EntityInformation> player_;
    std::vector<std::shared_ptr<EntityInformation>> entities_;

    // Pair is starting frame
    std::queue<std::pair<unsigned int, CutsceneEvent>> events_;
};

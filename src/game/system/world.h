#pragma once

#include <vector>
#include <memory>

#include "base_entity.h"
#include "player.h"
#include "json.hpp"
#include "components/rendering/rendering_text.h"

class World {
/* This class is intended to store all world objects like walls.
 * This is to be able to move the collision logic into the player/enemy enities
 *
 * TODO Figure out if this is a good idea
 */
  public:

    class IWorldRead {
      public:
        static const std::vector<std::weak_ptr<const Collision>>& getCollisions(Collision::CollisionType coll_type);

        static util::Point getPlayerPosition();

        static std::weak_ptr<Player> getPlayer();

        static bool hasInteractTriggered();
    };

    class IWorldModify {
      public:
        static void addEntity(std::shared_ptr<BaseEntity> entity);
        static void removeEntity(std::shared_ptr<BaseEntity> entity);

        static void loadWorldFromJson(nlohmann::json j);
        static void loadWorldFromFile(std::string file);

        static nlohmann::json saveWorldToJson();
        static void saveWorldToFile(std::string file);

        static void update();

        static void clearWorld();

        static std::vector<std::shared_ptr<BaseEntity>>& getWorldObjects();

        static void triggerInterract();

        static void loadRoom(std::string room_name, int entrance_id);
    };

    template <class T>
    static T getInstance();

    World(const World&) = delete;
    World(World&&) = delete;
    World operator=(const World&) = delete;
    World operator=(World&&) = delete;

  private:
    // Functions
    static World& getWorldInstance();
    std::weak_ptr<Player> getPlayer();

    nlohmann::json saveWorldToJson();
    void saveWorldToFile(std::string file);

    void update();

    void clearWorld();

    void addEntity(std::shared_ptr<BaseEntity> entity);
    void removeEntity(std::shared_ptr<BaseEntity> entity);
    void addPlayer(std::shared_ptr<Player> player);

    void loadWorldFromJson(nlohmann::json j);
    void loadWorldFromFile(std::string file);

    void loadRoom(std::string room_name, int entrance_id);

    std::vector<std::shared_ptr<BaseEntity>>::iterator deleteEntity(std::vector<std::shared_ptr<BaseEntity>>::iterator it);

    util::Point getPlayerPosition();

    std::vector<std::shared_ptr<BaseEntity>>& getWorldObjects();

    World() = default;


    // Data
    std::shared_ptr<Player> player_;

    std::shared_ptr<RenderableText> player_health_;
    std::shared_ptr<Transform> player_health_position_;

    std::vector<std::shared_ptr<BaseEntity>> world_objects_;

    std::array<std::vector<std::weak_ptr<const Collision>>, static_cast<int>(Collision::CollisionType::MAX_NUM)> collisions_;

    std::vector<util::Point> entrances_;

    std::optional<std::pair<std::string, int>> new_room_;

    bool interact_triggered_ = false;
};

template <> World::IWorldRead World::getInstance<World::IWorldRead>();
template <> World::IWorldModify World::getInstance<World::IWorldModify>();

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
    enum class Layer {
        BACKGROUND,

        // Counting outwards from MAIN
        BG_3,
        BG_2,
        BG_1,

        MAIN_BG,
        MAIN,
        MAIN_FG,

        // Counting outwards from MAIN
        FG_1,
        FG_2,
        FG_3,

        // HUD is treated differently
        HUD,
        MAX_LAYERS = HUD
    };

    class IWorldRead {
      public:
        static const std::vector<std::weak_ptr<const Collision>>& getCollisions(Collision::CollisionType coll_type);

        static util::Point getPlayerPosition();

        static std::weak_ptr<Player> getPlayer();
    };

    class IWorldModify {
      public:
        static void addEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer);
        static void removeEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer);

        static void loadWorldFromJson(nlohmann::json j);
        static void loadWorldFromFile(std::string file);

        static nlohmann::json saveWorldToJson();
        static void saveWorldToFile(std::string file);

        static void update();

        static void clearWorld();

        static std::vector<std::shared_ptr<BaseEntity>>& getWorldObjects(Layer layer = Layer::MAIN);
    };

    template <class T>
    static T getInstance();

    World(const World&) = delete;
    World(World&&) = delete;
    World operator=(const World&) = delete;
    World operator=(World&&) = delete;

    // For level editor
    static std::string getLayerString(Layer layer);

  private:
    static World& getWorldInstance();
    std::weak_ptr<Player> getPlayer();

    nlohmann::json saveWorldToJson();
    void saveWorldToFile(std::string file);

    void update();

    void clearWorld();

    void addEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer);
    void removeEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer);

    void loadWorldFromJson(nlohmann::json j);
    void loadWorldFromFile(std::string file);

    util::Point getPlayerPosition();

    std::vector<std::shared_ptr<BaseEntity>>& getWorldObjects(Layer layer = Layer::MAIN);

    World() = default;

    std::shared_ptr<Player> player_;

    std::shared_ptr<RenderableText> player_health_;
    std::shared_ptr<Transform> player_health_position_;

    void loadLayer(nlohmann::json json, Layer layer);
    nlohmann::json jsonifyLayer(Layer layer);

    std::array<std::vector<std::shared_ptr<BaseEntity>>, static_cast<int>(Layer::MAX_LAYERS) + 1> world_objects_;

    std::array<std::vector<std::weak_ptr<const Collision>>, static_cast<int>(Collision::CollisionType::MAX_NUM)> collisions_;
};

template <> World::IWorldRead World::getInstance<World::IWorldRead>();
template <> World::IWorldModify World::getInstance<World::IWorldModify>();

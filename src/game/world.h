#pragma once

#include <vector>
#include <memory>

#include "base_entity.h"
#include "player.h"

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

        MAX_LAYERS
    };

    static World& getInstance();

    void loadWorld(std::string file);

    void update();

    sf::Vector2i getPlayerPosition();

    std::vector<std::shared_ptr<BaseEntity>>& getWorldObjects(Layer layer = Layer::MAIN);

    void addEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer);
    void removeEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer);

    World(const World&) = delete;
    World(World&&) = delete;
    World operator=(const World&) = delete;
    World operator=(World&&) = delete;

    static std::string getLayerString(Layer layer);

  private:
    World() = default;

    std::shared_ptr<Player> player_;

    void loadLayer(nlohmann::json json, Layer layer);

    std::array<std::vector<std::shared_ptr<BaseEntity>>, static_cast<int>(Layer::MAX_LAYERS)> world_objects_;
};

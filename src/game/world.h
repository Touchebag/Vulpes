#pragma once

#include <vector>
#include <memory>

#include "base_entity.h"
#include "render.h"
#include "player.h"

class World {
/* This class is intended to store all world objects like walls.
 * This is to be able to move the collision logic into the player/enemy enities
 *
 * TODO Figure out if this is a good idea
 */
  public:
    static World& getInstance();

    void loadWorld(std::string file);

    void update();

    sf::Vector2i getPlayerPosition();

    std::vector<std::shared_ptr<BaseEntity>>& getWorldObjects();

    World(const World&) = delete;
    World(World&&) = delete;
    World operator=(const World&) = delete;
    World operator=(World&&) = delete;

  private:
    World() = default;

    std::shared_ptr<Player> player_;

    void loadLayer(nlohmann::json json, Render::Layer layer);

    std::vector<std::shared_ptr<BaseEntity>> world_objects_;
};

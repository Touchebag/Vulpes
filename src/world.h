#pragma once

#include <vector>
#include <memory>

#include "base_entity.h"

class World {
/* This class is intended to store all world objects like walls.
 * This is to be able to move the collision logic into the player/enemy enities
 *
 * TODO Figure out if this is a good idea
 */
  public:
    static World& getInstance();

    std::vector<std::shared_ptr<BaseEntity>>& getWorldObjects();

  private:
    std::vector<std::shared_ptr<BaseEntity>> worldObjects;
};

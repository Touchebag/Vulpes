#pragma once

#include <vector>
#include <memory>

#include "base_entity.h"
#include "player.h"
#include "nlohmann/json.hpp"
#include "components/rendering/rendering_text.h"
#include "camera.h"

class System;

class World {
/* This class is intended to store all world objects like walls.
 * This is to be able to move the collision logic into the player/enemy enities
 */
  public:
    friend System;

  private:
    // Functions
    std::weak_ptr<Player> getPlayer();

    nlohmann::json saveWorldToJson();
    void saveWorldToFile(std::string file);

    void update();

    void clearWorld();

    void addEntity(std::shared_ptr<BaseEntity> entity, std::optional<std::string> condition = std::nullopt);
    void removeEntity(std::shared_ptr<BaseEntity> entity);
    void addPlayer(std::shared_ptr<Player> player);

    void addCollideable(std::shared_ptr<Collideable> collideable);

    void loadWorldFromJson(nlohmann::json j);
    void loadWorldFromFile(std::string file);
    void loadWorldTemplate(std::string file);

    void addEntriesToWorld(nlohmann::json j, bool is_template = false);

    void loadRoom(std::string room_name, int entrance_id);

    void setEntrance(int entrance_id);
    void setShiftedPlayerPosition(Collideable::CollisionType c_type);

    void clearDeletedEntities();

    std::vector<std::shared_ptr<BaseEntity>>::iterator deleteEntity(std::vector<std::shared_ptr<BaseEntity>>::iterator it);

    util::Point getPlayerPosition();

    std::vector<std::shared_ptr<BaseEntity>>& getWorldObjects();

    // Data
    std::shared_ptr<Player> player_;

    std::shared_ptr<ComponentStore> player_health_ = std::make_shared<ComponentStore>();

    std::vector<std::shared_ptr<BaseEntity>> world_objects_;
    // Temporary storage to delete entities simultaneuosly at frame end
    std::vector<std::shared_ptr<BaseEntity>> deleted_objects_;

    std::array<std::vector<std::weak_ptr<const Collideable>>, static_cast<int>(Collideable::CollisionType::MAX_NUM)> collideables_;

    std::vector<util::Point> entrances_;

    std::optional<std::pair<std::string, int>> new_room_;

    // Loaded template stuff
    std::vector<std::string> template_file_names_;
    std::unordered_set<std::shared_ptr<BaseEntity>> template_objects_;

    // Conditional entities used for saving world
    std::vector<std::weak_ptr<BaseEntity>> conditional_entities_;

    // Meta data
    // Needed for saving world to file
    // TODO Store all meta data directly as json?
    std::string current_room_name_;

    std::optional<Camera::CameraBoundingBox> camera_box_;
};

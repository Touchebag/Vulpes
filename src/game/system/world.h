#pragma once

#include <vector>
#include <memory>
#include <set>

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

    World();

  private:
    // Functions
    std::weak_ptr<Player> getPlayer();

    nlohmann::json saveWorldToJson();
    void saveWorldToFile(const std::string& file);

    void update();

    void clearWorld();

    void addEntity(std::shared_ptr<BaseEntity> entity, std::optional<std::string> condition = std::nullopt);
    void removeEntity(std::shared_ptr<BaseEntity> entity);
    void addPlayer(std::shared_ptr<Player> player);

    void addCollideable(std::shared_ptr<Collideable> collideable);

    void loadWorldFromJson(nlohmann::json j);
    void loadWorldFromFile(const std::string& file);
    void loadWorldFromJsonInternal(nlohmann::json j, std::unordered_set<std::string>& templates, bool is_template);

    void addEntriesToWorld(nlohmann::json j, bool is_template = false);

    void loadRoom(const std::string& room_name, unsigned int entrance_id);

    void setEntrance(unsigned int entrance_id);
    void setShiftedPlayerPosition(Collideable::CollisionType c_type);

    void clearDeletedEntities();

    std::vector<std::shared_ptr<BaseEntity>>::iterator deleteEntity(std::vector<std::shared_ptr<BaseEntity>>::iterator it);

    util::Vec2i getPlayerPosition();

    std::vector<std::shared_ptr<BaseEntity>>& getWorldObjects();

    // Data
    std::shared_ptr<Player> player_;

    std::shared_ptr<ComponentStore> player_health_ = std::make_shared<ComponentStore>();

    // Used to tell if we are currently iterating through entity list
    bool update_loop_active_ = false;

    std::vector<std::shared_ptr<BaseEntity>> world_objects_ = {};
    // Temporary storage to add/delete entities simultaneously at frame end
    std::vector<std::shared_ptr<BaseEntity>> added_objects_ = {};
    std::vector<std::shared_ptr<BaseEntity>> deleted_objects_ = {};

    std::unordered_map<Collideable::CollisionType, std::vector<std::weak_ptr<const Collideable>>> collideables_ = {};

    std::vector<util::Vec2i> entrances_ = {};

    std::optional<std::pair<std::string, unsigned int>> new_room_ = {};

    // Loaded template stuff
    std::vector<std::string> template_file_names_;
    std::unordered_set<std::shared_ptr<BaseEntity>> template_objects_;

    // Conditional entities used for saving world
    std::vector<std::weak_ptr<BaseEntity>> conditional_entities_;

    std::map<std::string, std::shared_ptr<BaseEntity>> getEntitesByTags(const std::set<std::string>& tags);

    // Meta data
    // Needed for saving world to file
    // TODO Store all meta data directly as json?
    std::string current_room_name_;
    std::optional<nlohmann::json> layer_data_;

    std::optional<Camera::CameraBoundingBox> camera_box_;
};

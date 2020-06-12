#include "world.h"

#include "utils/log.h"
#include "utils/file.h"
#include "system/render.h"

template <>
World::IWorldRead World::getInstance<World::IWorldRead>() {
    return World::IWorldRead();
}

template <>
World::IWorldModify World::getInstance<World::IWorldModify>() {
    return World::IWorldModify();
}

World& World::getWorldInstance() {
    static World world;

    return world;
}

std::vector<std::shared_ptr<BaseEntity>>& World::getWorldObjects() {
    return world_objects_;
}

void World::update() {
    // TODO Check for death
    player_->update();

    for (auto it = world_objects_.begin();
              it != world_objects_.end();
              ) {
        (*it)->update();

        if ((*it)->death_ && (*it)->death_->isDead()) {
            it = deleteEntity(it);
        } else {
            ++it;
        }
    }

    if (player_->damageable_) {
        player_health_->setText(std::to_string(player_->damageable_->getHealth()));
    }
}

util::Point World::getPlayerPosition() {
    return player_->trans_->getPosition();
}

void World::clearWorld() {
    world_objects_.clear();

    player_ = nullptr;
}

void World::loadWorldFromFile(std::string path) {
    std::optional<nlohmann::json> j = File::loadRoom(path);

    if (!j) {
        LOGE("Unable to load json from file %s", path.c_str());
        exit(EXIT_FAILURE);
    }

    loadWorldFromJson(j.value());
}

void World::loadWorldFromJson(nlohmann::json j) {
    if (!j.contains("entities")) {
        LOGE("entities not found, exiting");
        exit(EXIT_FAILURE);
    }

    clearWorld();

    for (auto it : j["entities"]) {
        auto ent = BaseEntity::createFromJson(it);
        addEntity(ent);
    }

    player_ = Player::createFromJson(j["player"]);

    Render::getInstance().addEntity(player_->renderableEntity_);

    // Health HUD
    player_health_position_ = std::make_shared<Transform>();
    player_health_position_->setPosition(50, 50);

    player_health_ = std::make_shared<RenderableText>(player_health_position_);
    player_health_->setColor(sf::Color::Green);
    player_health_->setLayer(RenderableEntity::Layer::HUD);

    Render::getInstance().addEntity(player_health_);
}

void World::saveWorldToFile(std::string file) {
    nlohmann::json j = saveWorldToJson();

    if (File::writeJsonToFile(file, j)) {
        LOGD("World saved successfully");
    } else {
        LOGE("Failed to save json to file");
    }
}

nlohmann::json World::saveWorldToJson() {
    nlohmann::json j;

    nlohmann::json json_object_list;

    for (auto it : world_objects_) {
        if (auto object = it->outputToJson()) {
            // Do not store if HUD object
            if (!(it->renderableEntity_) || (it->renderableEntity_->getLayer() != RenderableEntity::Layer::HUD)) {
                json_object_list.push_back(*object);
            }
        }
    }

    j["entities"] = json_object_list;

    if (player_) {
        auto player_json = player_->outputToJson();

        if (!player_json) {
            LOGE("Failed to store player");
            return {};
        }

        j["player"] = player_json.value();
    }

    return j;
}

void World::addEntity(std::shared_ptr<BaseEntity> entity) {
    world_objects_.push_back(entity);

    auto coll = entity->collision_;
    if (coll) {
        collisions_[static_cast<int>(coll->getType())].push_back(coll);
    }

    Render::getInstance().addEntity(entity->renderableEntity_);
}

void World::removeEntity(std::shared_ptr<BaseEntity> entity) {
    std::vector<std::shared_ptr<BaseEntity>>::iterator it = std::find(world_objects_.begin(), world_objects_.end(), entity);
    if (it != world_objects_.end()) {
        deleteEntity(it);
    }
}

std::vector<std::shared_ptr<BaseEntity>>::iterator World::deleteEntity(std::vector<std::shared_ptr<BaseEntity>>::iterator entity_it) {
    auto ret_it = world_objects_.erase(entity_it);

    for (auto& coll_type : collisions_) {
        for (auto it = coll_type.begin(); it != coll_type.end();) {
            if (it->expired()) {
                it = coll_type.erase(it);
            } else {
                it++;
            }
        }
    }

    return ret_it;
}

std::weak_ptr<Player> World::getPlayer() {
    return player_;
}

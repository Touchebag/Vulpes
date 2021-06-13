#include "world.h"

#include "utils/log.h"
#include "utils/file.h"
#include "system/system.h"

std::vector<std::shared_ptr<BaseEntity>>& World::getWorldObjects() {
    return world_objects_;
}

void World::update() {
    if (System::getCutscene()) {
        System::getCutscene()->update();
    }

    // TODO Check for death
    player_->update();

    for (auto it = world_objects_.begin();
              it != world_objects_.end();
              ) {
        (*it)->update();

        if ((*it)->getComponent<Death>() && (*it)->getComponent<Death>()->isDead()) {
            // Spawn any death entities
            if (auto death_entity_json = (*it)->getComponent<Death>()->getDeathEntityJson()) {
                auto death_entity = BaseEntity::createFromJson(death_entity_json.value());

                if ((*it)->getComponent<Transform>() && death_entity->getComponent<Transform>()) {
                    death_entity->getComponent<Transform>()->setPosition((*it)->getComponent<Transform>()->getPosition());
                }

                addEntity(death_entity);
            }

            // Set any death flags
            auto env = System::getEnvironment();
            for (auto flag : (*it)->getComponent<Death>()->getFlags()) {
                env->setFlag(flag);
            }

            it = deleteEntity(it);
        } else {
            ++it;
        }
    }

    if (player_->getComponent<Damageable>()) {
        if (auto health_text = std::dynamic_pointer_cast<RenderableText>(player_health_->getComponent<RenderableEntity>())) {
            health_text->setText(std::to_string(player_->getComponent<Damageable>()->getHealth()));
        } else {
            LOGW("Unable to cast health text HUD");
        }
    }

    // Do not load a new room if cutscene is playing
    if (new_room_ && !System::getCutscene()->isCutscenePlaying()) {
        loadRoom(new_room_->first, new_room_->second);
        new_room_.reset();
    }

    // Reset interaction trigger at end of frame
    interact_triggered_ = false;

    // Delete all expired entities at end of frame
    clearDeletedEntities();
}

util::Point World::getPlayerPosition() {
    return player_->getComponent<Transform>()->getPosition();
}

void World::clearWorld() {
    world_objects_.clear();

    for (auto& it : collideables_) {
        it.clear();
    }

    entrances_.clear();
}

void World::addEntriesToWorld(nlohmann::json j) {
    if (j.contains("entrances")) {
        entrances_.resize(j["entrances"].size());
        for (auto it : j["entrances"]) {
            if (!(it.contains("id") && it.contains("pos_x") && it.contains("pos_y"))) {
                throw std::invalid_argument("entrances must contain id, pos_x and pos_y");
            }

            long long unsigned int id = it["id"].get<int>();
            if (id < entrances_.size()) {
                entrances_[id] = util::Point(it["pos_x"].get<int>(), it["pos_y"].get<int>());
            }
        }
    }

    if (j.contains("camera")) {
        auto jcam = j["camera"];
        if (!(jcam.contains("left") &&
             jcam.contains("right") &&
             jcam.contains("top") &&
             jcam.contains("bottom"))) {
            LOGW("World: missing camera parameters");
            camera_box_.reset();
        } else {
            camera_box_ = {jcam["left"], jcam["right"], jcam["top"], jcam["bottom"]};
        }
    } else {
        LOGV("World: missing camera data");
        camera_box_.reset();
    }

    // Camera margins
    if (camera_box_) {
        System::getCamera()->setCameraBox(camera_box_.value());
    } else {
        System::getCamera()->setCameraBox({0.0f, 0.0f, 0.0f, 0.0f});
    }

    auto env = System::getEnvironment();

    for (auto it : j["entities"]) {
        bool should_create = true;

        // If condition exists it must be set to add entity
        if (it.contains("condition")) {
            auto condition = it["condition"].get<std::string>();
            should_create = env->getFlag(condition);
        }

        if (should_create) {
            auto ent = BaseEntity::createFromJson(it);
            addEntity(ent);
        }
    }

    // Shaders
    if (j.contains("shaders")) {
        System::getRender()->loadLayerShaders(j["shaders"]);
    }
}

void World::loadWorldTemplate(std::string file) {
    auto j_opt = File().loadRoomTemplate(file);

    if (!j_opt) {
        LOGE("Unable to load template from file");
        exit(EXIT_FAILURE);
    }

    auto j = j_opt.value();

    // Conditionally load template
    if (j.contains("condition") && !System::getEnvironment()->getFlag(j["condition"])) {
        return;
    }

    if (j.contains("background")) {
        System::getRender()->setBackground(j["background"]);
    }

    // Recursively load templates
    if (j.contains("templates")) {
        for (auto temp : j["templates"]) {
            loadWorldTemplate(temp);
        }
    }

    addEntriesToWorld(j);
}

void World::loadWorldFromFile(std::string path) {
    std::optional<nlohmann::json> j = File().loadRoom(path);

    if (!j) {
        LOGE("Unable to load json from file %s", path.c_str());
        exit(EXIT_FAILURE);
    }

    current_room_name_ = path;

    loadWorldFromJson(j.value());
}

void World::loadWorldFromJson(nlohmann::json j) {
    if (!j.contains("entities")) {
        LOGE("entities not found, exiting");
        exit(EXIT_FAILURE);
    }

    clearWorld();
    System::getRender()->clearLayerShaders();

    if (j.contains("templates")) {
        for (auto temp : j["templates"]) {
            loadWorldTemplate(temp);
        }
    }

    if (j.contains("background")) {
        System::getRender()->setBackground(j["background"]);
    }

    addEntriesToWorld(j);

    // Don't reload player between rooms
    if (!player_) {
        nlohmann::json player_json;
        player_json["Entity"] = "player";
        player_ = Player::createFromJson(player_json);
        addPlayer(player_);
    }

    // Health HUD
    player_health_->setComponent<Transform>(std::make_shared<Transform>(player_health_));
    player_health_->getComponent<Transform>()->setPosition(50, 50);

    player_health_->setComponent<RenderableEntity>(std::make_shared<RenderableText>(player_health_));
    player_health_->getComponent<RenderableEntity>()->setColor(sf::Color::Green);
    player_health_->getComponent<RenderableEntity>()->setLayer(INT_MAX);

    System::getRender()->addEntity(player_health_->getComponent<RenderableEntity>());
}

void World::saveWorldToFile(std::string file) {
    nlohmann::json j = saveWorldToJson();

    file = File::appendSuffix(file, ".json");

    if (File().writeJsonToFile(file, j)) {
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
            if (!(it->getComponent<RenderableEntity>()) || (it->getComponent<RenderableEntity>()->getLayer() != INT_MAX)) {
                json_object_list.push_back(*object);
            }
        }
    }

    j["entities"] = json_object_list;

    if (!entrances_.empty()) {
        nlohmann::json entrance_list;

        for (int i = 0; i < static_cast<int>(entrances_.size()); i++) {
            nlohmann::json j_entrance;
            auto pos = entrances_.at(i);

            j_entrance["id"] = i;
            j_entrance["pos_x"] = pos.x;
            j_entrance["pos_y"] = pos.y;

            entrance_list.push_back(j_entrance);
        }

        j["entrances"] = entrance_list;
    }

    if (camera_box_) {
        nlohmann::json j_camera;
        auto camera = camera_box_.value();

        j_camera["top"] = camera.top_margin;
        j_camera["bottom"] = camera.bottom_margin;
        j_camera["left"] = camera.left_margin;
        j_camera["right"] = camera.right_margin;

        j["camera"] = j_camera;
    }

    return j;
}

void World::addEntity(std::shared_ptr<BaseEntity> entity) {
    world_objects_.push_back(entity);

    auto coll = entity->getComponent<Collision>();
    if (coll && coll->getCollideable()) {
        addCollideable(coll->getCollideable());
    }

    if (auto render = entity->getComponent<RenderableEntity>()) {
        System::getRender()->addEntity(render);
    }
}

void World::addCollideable(std::shared_ptr<Collideable> collideable) {
    collideables_[static_cast<int>(collideable->getType())].push_back(collideable);
}

void World::removeEntity(std::shared_ptr<BaseEntity> entity) {
    std::vector<std::shared_ptr<BaseEntity>>::iterator it = std::find(world_objects_.begin(), world_objects_.end(), entity);
    if (it != world_objects_.end()) {
        deleteEntity(it);
    }
}

void World::addPlayer(std::shared_ptr<Player> player) {
    auto coll = player->getComponent<Collision>();

    if (coll && coll->getCollideable()) {
        addCollideable(coll->getCollideable());
    }

    System::getRender()->setPlayer(player_->getComponent<RenderableEntity>());
}

void World::loadRoom(std::string room_name, int entrance_id) {
    loadWorldFromFile(room_name);

    setEntrance(entrance_id);
}

void World::setShiftedPlayerPosition(Collideable::CollisionType ctype) {
    auto world_colls = System::IWorldRead::getCollideables(ctype);
    auto p_trans = player_->getComponent<Transform>();

    for (auto it = world_colls.begin(); it != world_colls.end(); ++it) {
        if (player_->getComponent<Collision>()->collides(*it)) {
            // If this entrance would put the player inside an object
            // move player to spawn on top of object
            auto other_coll = it->lock();
            auto other_hbox = other_coll->getHitbox();
            auto other_trans = other_coll->getTransform().lock();

            auto new_y_pos = other_trans->getY() - (other_hbox->height_ / 2) -
                player_->getComponent<Collision>()->getCollideable()->getHitbox()->height_ / 2;

            p_trans->setPosition(p_trans->getX(), new_y_pos);
        }
    }
}

void World::setEntrance(int entrance_id) {
    if (entrance_id < static_cast<int>(entrances_.size())) {
        auto p_trans = player_->getComponent<Transform>();
        p_trans->setPosition(entrances_.at(entrance_id));

        // This is to avoid spawning inside objects
        setShiftedPlayerPosition(Collideable::CollisionType::STATIC);
        setShiftedPlayerPosition(Collideable::CollisionType::SEMI_SOLID);

        auto view = System::getCamera()->getView();
        view.x_pos = static_cast<float>(p_trans->getX());
        view.y_pos = static_cast<float>(p_trans->getY());
        System::getCamera()->setView(view);
    } else {
        throw std::invalid_argument("Entrance id not found");
    }
}

std::vector<std::shared_ptr<BaseEntity>>::iterator World::deleteEntity(std::vector<std::shared_ptr<BaseEntity>>::iterator entity_it) {
    deleted_objects_.push_back(*entity_it);

    return world_objects_.erase(entity_it);
}

void World::clearDeletedEntities() {
    deleted_objects_.clear();

    for (auto& coll_type : collideables_) {
        for (auto it = coll_type.begin(); it != coll_type.end();) {
            if (it->expired()) {
                it = coll_type.erase(it);
            } else {
                it++;
            }
        }
    }
}

std::weak_ptr<Player> World::getPlayer() {
    return player_;
}

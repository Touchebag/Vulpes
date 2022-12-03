#include "world.h"

#include "utils/log.h"
#include "utils/file.h"
#include "system/system.h"

#include "event_triggers/add_entity.h"

std::vector<std::shared_ptr<BaseEntity>>& World::getWorldObjects() {
    return world_objects_;
}

void World::update() {
    auto cutscene = System::getCutscene();

    if (cutscene) {
        cutscene->update();

        // If cutscene is over, clear from system
        if (!cutscene->isActive()) {
            System::clearCutscene();
        }
    }

    // Check cutscene again to ensure transitions happen on same frame
    if (!cutscene) {
        if (new_room_) {
            loadRoom(new_room_->first, new_room_->second);
            new_room_.reset();
        } else {
            // TODO Check for death
            if (player_) {
                player_->update();
            }

            for (auto it = world_objects_.begin();
                      it != world_objects_.end();
                      ) {
                (*it)->update();

                if ((*it)->getComponent<Death>() && (*it)->getComponent<Death>()->isDead()) {
                    // Spawn any death entities
                    if (auto death_entity_json = (*it)->getComponent<Death>()->getDeathEntityJson()) {
                        auto death_entity = BaseEntity::createFromJson(death_entity_json.value());

                        if ((*it)->getComponent<Transform>() && death_entity->getComponent<Transform>()) {
                            auto new_position = (*it)->getComponent<Transform>()->getPosition();
                            death_entity->getComponent<Transform>()->setPosition(new_position);
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

            if (player_ && player_->getComponent<Damageable>()) {
                if (auto health_text = std::dynamic_pointer_cast<RenderingText>(player_health_->getComponent<Rendering>())) {
                    health_text->setText(std::to_string(player_->getComponent<Damageable>()->getHealth()));
                } else {
                    LOGW("Unable to cast health text HUD");
                }
            }

            // TODO Move to in between world_objects update and deferred objects update
            System::getEnvironment()->triggerConditionalEvents();

            // Delete all expired entities at end of frame
            clearDeletedEntities();
        }
    }
}

util::Vec2i World::getPlayerPosition() {
    return player_->getComponent<Transform>()->getPosition();
}

void World::clearWorld() {
    world_objects_.clear();

    System::getEnvironment()->clearConditionalEvents();
    System::getRender()->clearLayers();

    for (auto& it : collideables_) {
        it.clear();
    }

    entrances_.clear();

    template_file_names_.clear();
    template_objects_.clear();

    conditional_entities_.clear();
}

void World::addEntriesToWorld(nlohmann::json j, bool is_template) {
    if (j.contains("entrances")) {
        entrances_.resize(j["entrances"].size());
        for (auto it : j["entrances"]) {
            if (!(it.contains("id") && it.contains("pos_x") && it.contains("pos_y"))) {
                throw std::invalid_argument("entrances must contain id, pos_x and pos_y");
            }

            long long unsigned id = it["id"].get<long long unsigned>();
            if (id < entrances_.size()) {
                entrances_[id] = util::Vec2i(it["pos_x"].get<int>(), it["pos_y"].get<int>());
            }
        }
    }

    if (System::getCamera()) {
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
    }

    for (auto it : j["entities"]) {
        auto ent = BaseEntity::createFromJson(it);

        std::optional<std::string> condition = std::nullopt;
        if (it.contains("condition")) {
            condition = {it["condition"].get<std::string>()};
        }

        addEntity(ent, condition);

        if (is_template) {
            template_objects_.insert(ent);
        }
    }
}

void World::loadWorldFromFile(const std::string& path) {
    std::optional<nlohmann::json> j = File().loadRoom(path);

    if (!j) {
        LOGE("Unable to load json from file %s", path.c_str());
        exit(EXIT_FAILURE);
    }

    current_room_name_ = path;

    loadWorldFromJson(j.value());
}

void World::loadWorldFromJsonInternal(nlohmann::json j, std::unordered_set<std::string>& templates, bool is_template) {
    if (!is_template) {
        layer_data_ = std::nullopt;
    }

    // Conditionally load template
    if (j.contains("condition") && !System::getEnvironment()->getFlag(j["condition"])) {
        return;
    }

    // Load templates first to allow override
    if (j.contains("templates")) {
        for (auto temp : j["templates"]) {
            auto file_name = temp.get<std::string>();

            // Only load each template once
            if (templates.count(file_name) > 0) {
                continue;
            }

            if (auto j_opt = File().loadRoomTemplate(temp)) {
                templates.insert(file_name);
                loadWorldFromJsonInternal(j_opt.value(), templates, true);
                template_file_names_.push_back(file_name);
            } else {
                LOGE("Unable to load template from file");
                exit(EXIT_FAILURE);
            }
        }
    }

    if (j.contains("layers")) {
        auto j_layers = j["layers"];
        std::vector<double> layers;

        // Store layer data (for saving world) if base file
        if (!is_template) {
            layer_data_ = std::optional<nlohmann::json>(j["layers"]);
        }

        if (j_layers.contains("bg")) {
            for (auto it : j_layers["bg"]) {
                layers.push_back(it.get<double>());
            }

            System::getRender()->setBackgroundLayers(layers);
            layers.clear();
        }

        if (j_layers.contains("fg")) {
            for (auto it : j_layers["fg"]) {
                layers.push_back(it.get<double>());
            }

            System::getRender()->setForegroundLayers(layers);
        }
    }

    if (j.contains("background")) {
        System::getRender()->setBackground(j["background"]);
    }

    addEntriesToWorld(j, is_template);
}

void World::loadWorldFromJson(nlohmann::json j) {
    clearWorld();

    std::unordered_set<std::string> templates;
    // First json instance is not considered a template
    loadWorldFromJsonInternal(j, templates, false);

    // Don't reload player between rooms
    if (!player_) {
        nlohmann::json player_json;
        player_json["Entity"] = "player";
        player_ = Player::createFromJson(player_json);
    }

    // Re-add player to ensure collideable are added correctly
    addPlayer(player_);

    // Health HUD
    player_health_->setComponent<Transform>(std::make_shared<Transform>(player_health_));
    player_health_->getComponent<Transform>()->setPosition(50, 50);

    player_health_->setComponent<Rendering>(std::make_shared<RenderingText>(player_health_));
    player_health_->getComponent<Rendering>()->setColor(sf::Color::Green);
    player_health_->getComponent<Rendering>()->setLayer(INT_MAX);

    System::getRender()->addEntity(player_health_->getComponent<Rendering>());
}

void World::saveWorldToFile(const std::string& file) {
    nlohmann::json j = saveWorldToJson();

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
        // Only save if it's not a template entity
        if (template_objects_.count(it) == 0) {
            if (auto object = it->outputToJson()) {
                // Do not store if HUD object
                if (!(it->getComponent<Rendering>()) || (it->getComponent<Rendering>()->getLayer() != INT_MAX)) {
                    json_object_list.push_back(*object);
                }
            }
        }
    }

    for (auto it : conditional_entities_) {
        if (auto ent = it.lock()) {
            // Don't output as conditional if already added as normal entity
            if (std::find(world_objects_.begin(), world_objects_.end(), ent) == world_objects_.end()) {
                if (auto object = ent->outputToJson()) {
                    json_object_list.push_back(*object);
                }
            }
        }
    }

    j["entities"] = json_object_list;

    if (!entrances_.empty()) {
        nlohmann::json entrance_list;

        for (int i = 0; i < static_cast<int>(entrances_.size()); i++) {
            nlohmann::json j_entrance;
            auto pos = entrances_.at(static_cast<unsigned long>(i));

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

    if (!template_file_names_.empty()) {
        j["templates"] = template_file_names_;
    }

    if (layer_data_) {
        j["layers"] = layer_data_.value();
    }

    return j;
}

void World::addEntity(std::shared_ptr<BaseEntity> entity, std::optional<std::string> condition) {
    if (!condition || System::getEnvironment()->getFlag(condition.value())) {
        world_objects_.push_back(entity);

        auto coll = entity->getComponent<Collision>();
        if (coll && coll->getCollideable()) {
            addCollideable(coll->getCollideable());
        }

        if (auto render = entity->getComponent<Rendering>()) {
            System::getRender()->addEntity(render);
        }
    } else {
        auto evt_trigger = std::make_shared<event_triggers::AddEntity>(entity);
        System::getEnvironment()->addConditionalEvent(condition.value(), evt_trigger);
        conditional_entities_.push_back(entity);
    }
}

void World::addCollideable(std::shared_ptr<Collideable> collideable) {
    collideables_[static_cast<unsigned long>(collideable->getType())].push_back(collideable);
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

    System::getRender()->setPlayer(player_->getComponent<Rendering>());
}

void World::loadRoom(const std::string& room_name, unsigned int entrance_id) {
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

            int new_y_pos = other_trans->getY() - static_cast<int>(other_hbox->height_ / 2) -
                static_cast<int>(player_->getComponent<Collision>()->getCollideable()->getHitbox()->height_ / 2);

            p_trans->setPosition(p_trans->getX(), new_y_pos);
        }
    }
}

void World::setEntrance(unsigned int entrance_id) {
    if (entrance_id < static_cast<unsigned int>(entrances_.size())) {
        auto p_trans = player_->getComponent<Transform>();
        p_trans->setPosition(entrances_.at(entrance_id));

        // This is to avoid spawning inside objects
        setShiftedPlayerPosition(Collideable::CollisionType::STATIC);

        if (System::getCamera()) {
            auto view = System::getCamera()->getView();
            view.x_pos = static_cast<float>(p_trans->getX());
            view.y_pos = static_cast<float>(p_trans->getY());
            System::getCamera()->setView(view);
        }
    } else {
        throw std::invalid_argument("Entrance id not found");
    }
}

std::vector<std::shared_ptr<BaseEntity>>::iterator World::deleteEntity(std::vector<std::shared_ptr<BaseEntity>>::iterator entity_it) {
    deleted_objects_.push_back(*entity_it);

    auto template_it = template_objects_.find(*entity_it);
    if (template_it != template_objects_.end()) {
        template_objects_.erase(template_it);
    }

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

std::map<std::string, std::shared_ptr<BaseEntity>> World::getEntitesByTags(const std::set<std::string>& tags) {
    std::map<std::string, std::shared_ptr<BaseEntity>> ret_map;

    if (player_) {
        auto tag = player_->getTag();

        if (!tag.empty() && tags.count(tag) > 0) {
            ret_map.insert_or_assign(tag, player_);
        }
    }

    for (auto it : world_objects_) {
        auto tag = it->getTag();

        // If current tag is in requested list, add to return map
        if (!tag.empty() && tags.count(tag) > 0) {
            ret_map.insert_or_assign(tag, it);
        }
    }

    return ret_map;
}

std::weak_ptr<Player> World::getPlayer() {
    return player_;
}

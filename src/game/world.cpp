#include "world.h"

#include "log.h"
#include "file.h"

World& World::getInstance() {
    static World world;

    return world;
}

std::vector<std::shared_ptr<BaseEntity>>& World::getWorldObjects() {
    return world_objects_;
}

void World::update() {
    player_->update();

    for (auto it = world_objects_.begin(); it != world_objects_.end(); ++it) {
        (*it)->update();
    }
}

sf::Vector2i World::getPlayerPosition() {
    return player_->getPosition();
}

void World::loadWorld(std::string path) {
    std::optional<nlohmann::json> json_file = file::loadJson(path);

    if (!json_file) {
        LOGE("Unable to load json from file %s", path.c_str());
        exit(EXIT_FAILURE);
    }

    nlohmann::json j = json_file.value();

    if (!j.contains("main")) {
        LOGE("Main not found, exiting");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < static_cast<int>(Render::Layer::MAX_LAYERS); ++i) {
        Render::Layer layer = static_cast<Render::Layer>(i);
        loadLayer(j[Render::getLayerString(layer)], layer);
    }

    player_ = std::make_shared<Player>();
    auto j_player = j["player"];

    int xpos;
    int ypos;
    try {
        xpos = j_player["xpos"].get<int>();
        ypos = j_player["ypos"].get<int>();
    } catch (nlohmann::json::type_error& e) {
        LOGW("Player position not found, using default values");
        xpos = 0;
        ypos = 0;
    }

    player_->setPosition(xpos, ypos);
    player_->setHitbox(50, 200);
    player_->loadTexture("Player.png");
    player_->loadSpriteMap("Player.txt");

    Render::getInstance().addEntity(player_, Render::Layer::MAIN);
}

void World::loadLayer(nlohmann::json json, Render::Layer layer) {
    for (auto it : json) {
        std::shared_ptr<BaseEntity> ent = std::make_shared<BaseEntity>();
        ent->loadFromJson(it);

        // Only objects from main layer are interactable
        if (layer == Render::Layer::MAIN) {
            world_objects_.push_back(ent);
        }

        Render::getInstance().addEntity(ent, layer);
    }
}


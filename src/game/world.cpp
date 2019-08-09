#include "world.h"

#include "log.h"
#include "file.h"
#include "render.h"

namespace {

std::map<World::Layer, std::string> LayerStringMap = {
    {World::Layer::BACKGROUND, "background"},
    {World::Layer::BG_3, "bg3"},
    {World::Layer::BG_2, "bg2"},
    {World::Layer::BG_1, "bg1"},
    {World::Layer::MAIN_BG, "main_bg"},
    {World::Layer::MAIN, "main"},
    {World::Layer::MAIN_FG, "main_fg"},
    {World::Layer::FG_1, "fg1"},
    {World::Layer::FG_2, "fg2"},
    {World::Layer::FG_3, "fg3"},
};

}

std::string World::getLayerString(Layer layer) {
    return LayerStringMap[layer];
}

World& World::getInstance() {
    static World world;

    return world;
}

std::vector<std::shared_ptr<BaseEntity>>& World::getWorldObjects(Layer layer) {
    return world_objects_[static_cast<int>(layer)];
}

void World::update() {
    player_->update();

    for (auto it = world_objects_[static_cast<int>(Layer::MAIN)].begin();
              it != world_objects_[static_cast<int>(Layer::MAIN)].end();
              ++it) {
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

    for (int i = 0; i < static_cast<int>(Layer::MAX_LAYERS); ++i) {
        Layer layer = static_cast<Layer>(i);
        loadLayer(j[getLayerString(layer)], layer);
    }

    player_ = std::make_shared<Player>();
    player_->loadFromJson(j["player"]);

    Render::getInstance().addEntity(player_->getRenderable(), Layer::MAIN);
}

void World::loadLayer(nlohmann::json json, Layer layer) {
    for (auto it : json) {
        std::shared_ptr<BaseEntity> ent = std::make_shared<BaseEntity>();
        ent->loadFromJson(it);

        addEntity(ent, layer);
    }
}

nlohmann::json World::jsonifyLayer(Layer layer) {
    nlohmann::json json_object_list;

    auto layerList = world_objects_[static_cast<int>(layer)];

    for (long unsigned int i = 0; i < layerList.size(); ++i) {
        auto object = layerList.at(i)->outputToJson();
        if (object) {
            json_object_list.push_back(*object);
        }
    }

    return json_object_list;
}

void World::saveWorld(std::string file) {
    nlohmann::json j;

    for (int i = 0; i < static_cast<int>(Layer::MAX_LAYERS); ++i) {
        j[getLayerString(static_cast<Layer>(i))] = jsonifyLayer(static_cast<Layer>(i));
    }

    auto player_json = player_->outputToJson();

    if (!player_json) {
        LOGE("Failed to store player");
        return;
    }

    j["player"] = player_json.value();

    if (file::storeJson(file, j)) {
        LOGD("World save successfully");
    } else {
        LOGE("Failed to save json to file");
    }
}

void World::addEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer) {
    world_objects_[static_cast<int>(layer)].push_back(entity);

    Render::getInstance().addEntity(entity->getRenderable(), layer);
}

void World::removeEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer) {
    auto& layer_list = world_objects_[static_cast<int>(layer)];
    layer_list.erase(std::remove(layer_list.begin(), layer_list.end(), entity), layer_list.end());

    Render::getInstance().removeEntity(entity->getRenderable(), layer);
}

std::weak_ptr<Player> World::getPlayer() {
    return player_;
}

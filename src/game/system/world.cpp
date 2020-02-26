#include "world.h"

#include "utils/log.h"
#include "utils/file.h"
#include "system/render.h"

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

util::Point World::getPlayerPosition() {
    return player_->trans_->getPosition();
}

void World::clearWorld() {
    for (auto& it : world_objects_) {
        it.clear();
    }

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
    if (!j.contains("main")) {
        LOGE("Main not found, exiting");
        exit(EXIT_FAILURE);
    }

    clearWorld();

    for (int i = 0; i < static_cast<int>(Layer::MAX_LAYERS); ++i) {
        Layer layer = static_cast<Layer>(i);
        auto layer_string = getLayerString(layer);
        if (j.contains(layer_string)) {
            loadLayer(j[layer_string], layer);
        }
    }

    player_ = std::make_shared<Player>();
    player_->loadFromJson(j["player"]);

    Render::getInstance().addEntity(player_->renderableEntity_, Layer::MAIN);
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

    for (int i = 0; i < static_cast<int>(Layer::MAX_LAYERS); ++i) {
        auto layer_string = getLayerString(static_cast<Layer>(i));
        j[layer_string] = jsonifyLayer(static_cast<Layer>(i));
    }

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

void World::addEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer) {
    world_objects_[static_cast<int>(layer)].push_back(entity);

    auto coll = entity->collision_;
    if (coll) {
        collisions_[static_cast<int>(coll->getType())].push_back(coll);
    }

    Render::getInstance().addEntity(entity->renderableEntity_, layer);
}

void World::removeEntity(std::shared_ptr<BaseEntity> entity, World::Layer layer) {
    auto& layer_list = world_objects_[static_cast<int>(layer)];
    layer_list.erase(std::remove(layer_list.begin(), layer_list.end(), entity), layer_list.end());
}

std::weak_ptr<Player> World::getPlayer() {
    return player_;
}

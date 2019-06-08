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

void World::loadWorld(std::string path) {
    std::optional<nlohmann::json> j = file::loadJson(path);

    if (j) {
        if (j.value().contains("main")) {
            loadLayer(j.value()["main"], Render::Layer::MAIN);
        } else {
            LOGE("Main not found, exiting");
            exit(EXIT_FAILURE);
        }
        if (j.value().contains("bg1")) {
            loadLayer(j.value()["bg1"], Render::Layer::BG_1);
        }
        if (j.value().contains("bg2")) {
            loadLayer(j.value()["bg2"], Render::Layer::BG_2);
        }
        if (j.value().contains("bg3")) {
            loadLayer(j.value()["bg3"], Render::Layer::BG_3);
        }
    }
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


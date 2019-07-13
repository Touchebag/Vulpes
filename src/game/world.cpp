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
        if (!j.value().contains("main")) {
            LOGE("Main not found, exiting");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < static_cast<int>(Render::Layer::MAX_LAYERS); ++i) {
            Render::Layer layer = static_cast<Render::Layer>(i);
            loadLayer(j.value()[Render::getLayerString(layer)], layer);
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


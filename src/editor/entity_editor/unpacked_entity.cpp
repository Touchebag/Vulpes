#include "unpacked_entity.h"

#include "utils/file.h"

#include "utils/log.h"

namespace {

struct EntityAssets {
    nlohmann::json entity;
    nlohmann::json state;
};

EntityAssets loadAllAssets(const std::string& entity_name) {
    auto dir_scope = File::pushDirectory(File::getEntityDir() / entity_name);

    EntityAssets assets;

    if (auto j_entity = File::loadEntityFromFile()) {
        assets.entity = j_entity.value();
    } else {
        LOGD("%s, entity not found", entity_name.c_str());
        throw std::invalid_argument("Entity not found");
    }

    if (auto j_state = File::loadStates()) {
        assets.state = j_state.value();
    }

    return assets;
}

} // namespace

UnpackedEntity UnpackedEntity::unpackEntity(const std::string& entity_name) {
    UnpackedEntity entity = UnpackedEntity{};

    entity.entity_name_ = entity_name;

    auto assets = loadAllAssets(entity_name);

    entity.state_view.unpack(assets.state);

    return entity;
}

void UnpackedEntity::draw(sf::RenderWindow& window) {
    state_view.draw(window);
}

void UnpackedEntity::handleKeyPress(sf::Event event) {
    state_view.handleKeyPress(event);
}

void UnpackedEntity::handleMouseClick(sf::Event event, Mouse mouse) {
    state_view.handleMouseClick(event, mouse.getMouseWorldPosition());
}

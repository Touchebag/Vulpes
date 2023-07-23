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

namespace entity_editor {

UnpackedEntity UnpackedEntity::unpackEntity(const std::string& entity_name) {
    UnpackedEntity entity = UnpackedEntity{};

    entity.entity_name_ = entity_name;

    auto assets = loadAllAssets(entity_name);

    entity.state_editor_.unpack(assets.state);

    return entity;
}

void UnpackedEntity::draw(sf::RenderWindow& window) {
    switch (entity_editor::EntityEditorCommon::getCurrentEditorView()) {
        case EntityEditorView::STATE:
            state_editor_.draw(window);
            break;
        case EntityEditorView::ANIMATION:
            animation_editor_.draw(window);
            break;
        default:
            LOGW("EntityEditor, no view selected")
            break;
    }
}

void UnpackedEntity::handleKeyPress(sf::Event event) {
    state_editor_.handleKeyPress(event);
}

void UnpackedEntity::handleMouseClick(sf::Event event, Mouse mouse) {
    if (!ImGui::GetIO().WantCaptureMouse) {
        state_editor_.handleMouseClick(event, mouse.getMouseWorldPosition());
    }
}

} // entity_editor

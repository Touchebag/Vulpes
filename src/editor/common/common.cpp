#include "common.h"

#include "utils/log.h"

namespace editor_common {

static CurrentEditor current_editor = CurrentEditor::LEVEL;

std::shared_ptr<BaseEntity> makeHudText(std::pair<int, int> position = {0, 0}) {
    std::shared_ptr<BaseEntity> text_element = std::make_shared<BaseEntity>();

    std::shared_ptr<Transform> trans = std::make_shared<Transform>(text_element->components_);
    std::shared_ptr<RenderingText> text = std::make_shared<RenderingText>(text_element->components_);
    text->setColor(sf::Color::Green);
    text->setLayer(INT_MAX);

    trans->setPosition(position.first, position.second);

    text_element->setComponent<Transform>(trans);
    text_element->setComponent<Rendering>(text);
    System::IWorldModify::addEntity(text_element);

    return text_element;
}

void setCurrentEditor(CurrentEditor editor) {
    current_editor = editor;
}

CurrentEditor getCurrentEditor() {
    return current_editor;
}

} // editor_common

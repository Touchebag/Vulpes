#include "common.h"

namespace editor_common {

std::shared_ptr<BaseEntity> makeHudText(std::pair<int, int> position = {0, 0}) {
    std::shared_ptr<BaseEntity> text_element = std::make_shared<BaseEntity>();

    std::shared_ptr<Transform> trans = std::make_shared<Transform>(text_element->components_);
    std::shared_ptr<RenderableText> text = std::make_shared<RenderableText>(text_element->components_);
    text->setColor(sf::Color::Green);
    text->setLayer(INT_MAX);

    trans->setPosition(position.first, position.second);

    text_element->setComponent<Transform>(trans);
    text_element->setComponent<RenderableEntity>(text);
    World::getInstance<World::IWorldModify>().addEntity(text_element);

    return text_element;
}

} // editor_common

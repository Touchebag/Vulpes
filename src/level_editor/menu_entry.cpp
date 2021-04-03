#include "menu_entry.h"

#include "utils/common.h"
#include "system/system.h"
#include "components/rendering/rendering_text.h"

namespace {

std::shared_ptr<BaseEntity> createTextEntity (std::string text_string, util::Point pos, sf::Color color) {
    std::shared_ptr<BaseEntity> text_entity = std::make_shared<BaseEntity>();

    std::shared_ptr<Transform> trans = std::make_shared<Transform>(text_entity->components_);
    trans->setPosition(pos.x, pos.y);

    std::shared_ptr<RenderableText> text = std::make_shared<RenderableText>(text_entity->components_);
    text->setColor(color);
    text->setText(text_string);

    text_entity->setComponent<Transform>(trans);
    text_entity->setComponent<RenderableEntity>(text);

    return text_entity;
}

}

MenuEntry::MenuEntry(const std::string& text, std::optional<Command::Commands> action) :
    text_(text),
    action_(action) {
}

std::string MenuEntry::getText() {
    return text_;
}

void MenuEntry::setText(const std::string& text) {
    text_ = text;
}

std::optional<Command::Commands> MenuEntry::getAction() {
    return action_;
}

void MenuEntry::setColor(sf::Color color) {
    color_ = color;
}

sf::Color MenuEntry::getColor() {
    return color_;
}

std::shared_ptr<MenuEntry> MenuEntry::selectOption(int option) {
    if (option < 1) {
        return {};
    }

    if (option < static_cast<int>(entries_.size()) + 1) {
        return entries_.at(option - 1);
    }

    return {};
}

void MenuEntry::addEntry(std::shared_ptr<MenuEntry> entry) {
    entries_.push_back(entry);
}

std::vector<std::shared_ptr<BaseEntity>> MenuEntry::draw() {
    std::vector<std::shared_ptr<BaseEntity>> menu_text_;

    auto title = createTextEntity(text_, {100, 100}, color_);

    menu_text_.push_back(title);
    title->getComponent<RenderableEntity>()->setLayer(INT_MAX);
    System::getRender()->addEntity(title->getComponent<RenderableEntity>());

    int i = 0;
    for (auto& it : entries_) {
        if (it) {
            std::string entry_text = std::to_string(i + 1) + ": " + it->getText();

            // Move each extra entry down one row
            util::Point position = {120, static_cast<int>(150 + (50 * i))};

            auto text_element = createTextEntity(entry_text, position, it->getColor());

            menu_text_.push_back(text_element);
            text_element->getComponent<RenderableEntity>()->setLayer(INT_MAX);
            System::getRender()->addEntity(text_element->getComponent<RenderableEntity>());
            ++i;
        }
    }

    return menu_text_;
}

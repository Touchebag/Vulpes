#include "menu_entry.h"

#include "utils/common.h"
#include "render.h"
#include "components/rendering/rendering_text.h"

namespace {

std::shared_ptr<BaseEntity> createTextEntity (std::string text_string, util::Point pos) {
    std::shared_ptr<BaseEntity> text_entity = std::make_shared<BaseEntity>();

    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    trans->setPosition(pos.x, pos.y);

    std::shared_ptr<RenderableText> text = std::make_shared<RenderableText>(trans);
    text->setColor(sf::Color::Green);
    text->setText(text_string);

    text_entity->trans_ = trans;
    text_entity->renderableEntity_ = text;

    return text_entity;
}

}

MenuEntry::MenuEntry(const std::string& text) :
    text_(text) {
}

std::string MenuEntry::getText() {
    return text_;
}

void MenuEntry::setText(const std::string& text) {
    text_ = text;
}

std::shared_ptr<MenuEntry> MenuEntry::selectOption(int option) {
    if (option < 1) {
        return {};
    }

    if (option < static_cast<int>(entries_.size()) + 1) {
        return entries_.at(option - 1).lock();
    }

    return {};
}

void MenuEntry::addEntry(std::weak_ptr<MenuEntry> entry) {
    entries_.push_back(entry);
}

std::vector<std::shared_ptr<BaseEntity>> MenuEntry::draw() {
    std::vector<std::shared_ptr<BaseEntity>> menu_text_;

    auto title = createTextEntity(text_, {100, 100});

    menu_text_.push_back(title);
    Render::getInstance().addEntity(title->renderableEntity_, World::Layer::HUD);

    int i = 0;
    for (auto& it : entries_) {
        if (auto it_ptr = it.lock()) {
            std::string entry_text = std::to_string(i + 1) + ": " + it_ptr->getText();

            // Move each extra entry down one row
            util::Point position = {120, static_cast<int>(150 + (50 * i))};

            auto text_element = createTextEntity(entry_text, position);

            menu_text_.push_back(text_element);
            Render::getInstance().addEntity(text_element->renderableEntity_, World::Layer::HUD);
            ++i;
        }
    }

    return menu_text_;
}

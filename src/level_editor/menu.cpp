#include "menu.h"

#include "render.h"
#include "components/rendering/rendering_text.h"

Menu::Menu() {
    auto ent = std::make_shared<MenuEntry>("A");
    addEntry(ent);
    current_entry = ent;

    ent = std::make_shared<MenuEntry>("B");
    addEntry(ent);

    draw();
}

void Menu::addEntry(std::shared_ptr<MenuEntry> entry) {
    entries_.push_back(entry);
}

void Menu::draw() {
    for (long long unsigned int i = 0; i < entries_.size(); ++i) {
        std::shared_ptr<BaseEntity> text_element = std::make_shared<BaseEntity>();

        std::shared_ptr<Transform> trans = std::make_shared<Transform>();
        std::shared_ptr<RenderableText> text = std::make_shared<RenderableText>(trans);
        text->setColor(sf::Color::Green);

        text->setText(std::to_string(i + 1) + ": " + entries_.at(i)->getText());

        // Move each extra entry down one row
        trans->setPosition(100, static_cast<int>(100 + (50 * i)));

        text_element->trans_ = trans;
        text_element->renderableEntity_ = text;

        menu_text_.push_back(text_element);
        Render::getInstance().addEntity(text_element->renderableEntity_, World::Layer::HUD);
    }
}

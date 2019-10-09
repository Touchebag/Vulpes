#include "menu.h"

#include "menu_entries.h"

Menu::Menu(std::shared_ptr<BaseEntity> current_entity) {
    // TODO Break out to separate file?
    if (current_entity) {
        auto main_entry = std::make_shared<MenuEntry>("Main");
        current_entry = main_entry;

        for (auto it : menu_entries::makeRenderableEntry(current_entity, main_entry)) {
            addEntry(it);
        }

        for (auto it : menu_entries::makeCollisionEntry(current_entity, main_entry)) {
            addEntry(it);
        }

        auto ent1 = std::make_shared<MenuEntry>("B");

        auto ent2 = std::make_shared<MenuEntry>("BA");
        ent1->addEntry(ent2);
        addEntry(ent2);
        ent2 = std::make_shared<MenuEntry>("BB");
        ent1->addEntry(ent2);
        addEntry(ent2);
        ent2 = std::make_shared<MenuEntry>("BC");
        ent1->addEntry(ent2);
        addEntry(ent2);

        addEntry(ent1);
        main_entry->addEntry(ent1);

        draw();
    }
}

std::optional<Command::Commands> Menu::selectOption(int option) {
    auto new_entry = current_entry->selectOption(option);

    if (new_entry) {
        if (auto action = new_entry->getAction()) {
            return action;
        }

        current_entry = new_entry;
        draw();
    }

    return std::nullopt;
}

void Menu::draw() {
    menu_text_ = current_entry->draw();
}

void Menu::addEntry(std::shared_ptr<MenuEntry> entry) {
    entries_.push_back(entry);
}

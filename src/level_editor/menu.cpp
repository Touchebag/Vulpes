#include "menu.h"

#include "menu_entries.h"

Menu::Menu(std::shared_ptr<BaseEntity> current_entity) {
    if (current_entity) {
        auto main_entry = std::make_shared<MenuEntry>("Main");
        current_entry = main_entry;
        main_entry_ = main_entry;

        main_entry->addEntry(menu_entries::makeRenderableEntry(current_entity));

        main_entry->addEntry(menu_entries::makeCollisionEntry(current_entity));

        main_entry->addEntry(menu_entries::makeMovableEntry(current_entity));

        main_entry->addEntry(menu_entries::makePhysicsEntry(current_entity));

        main_entry->addEntry(menu_entries::makeActionsEntry(current_entity));

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


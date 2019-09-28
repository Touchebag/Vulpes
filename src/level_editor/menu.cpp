#include "menu.h"

Menu::Menu() {
    // TODO Break out to separate file?
    auto main_entry = std::make_shared<MenuEntry>("Main");
    current_entry = main_entry;

    auto ent1 = std::make_shared<MenuEntry>("A");

    auto ent2 = std::make_shared<MenuEntry>("AA");
    ent1->addEntry(ent2);
    addEntry(ent2);
    ent2 = std::make_shared<MenuEntry>("AB");
    ent1->addEntry(ent2);
    addEntry(ent2);

    addEntry(ent1);
    main_entry->addEntry(ent1);

    ent1 = std::make_shared<MenuEntry>("B");

    ent2 = std::make_shared<MenuEntry>("BA");
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

void Menu::selectOption(int option) {
    auto new_entry = current_entry->selectOption(option);

    if (new_entry) {
        current_entry = new_entry;
        draw();
    }
}

void Menu::addEntry(std::shared_ptr<MenuEntry> entry) {
    entries_.push_back(entry);
}

void Menu::draw() {
    menu_text_ = current_entry->draw();
}

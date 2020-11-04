#include "menu.h"

#include "menu_entries.h"
#include "editor_loop/editor_environment.h"

Menu::Menu(std::weak_ptr<EditorEnvironment> editor_environment) :
    editor_env_(editor_environment) {
    auto editor_env = editor_env_.lock();
    if (editor_env->current_entity) {
        auto main_entry = std::make_shared<MenuEntry>("Main");

        main_entry->addEntry(menu_entries::makeRenderableEntry(editor_env->current_entity));

        main_entry->addEntry(menu_entries::makeCollisionEntry(editor_env->current_entity));

        main_entry->addEntry(menu_entries::makeMovableEntry(editor_env->current_entity));

        main_entry->addEntry(menu_entries::makePhysicsEntry(editor_env->current_entity));

        main_entry->addEntry(menu_entries::makeActionsEntry(editor_env->current_entity));

        current_entry_ = main_entry;
        main_entry_ = main_entry;

        draw();
    }
}

std::optional<Command::Commands> Menu::selectOption(int option) {
    auto new_entry = current_entry_->selectOption(option);

    if (new_entry) {
        if (auto action = new_entry->getAction()) {
            return action;
        }

        current_entry_ = new_entry;
        draw();
    }

    return std::nullopt;
}

void Menu::draw() {
    menu_text_ = current_entry_->draw();
}


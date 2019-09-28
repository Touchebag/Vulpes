#include "menu_entry.h"

MenuEntry::MenuEntry(const std::string& text) :
    text_(text) {
}

std::string MenuEntry::getText() {
    return text_;
}

void MenuEntry::setText(const std::string& text) {
    text_ = text;
}

void MenuEntry::selectOption(int option) {
}

void MenuEntry::addEntry(std::weak_ptr<MenuEntry> entry) {
    entries_.push_back(entry);
}

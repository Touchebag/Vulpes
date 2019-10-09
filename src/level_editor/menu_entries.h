
namespace menu_entries {

// TODO Break out common parts
std::vector<std::shared_ptr<MenuEntry>> makeRenderableEntry(
        std::shared_ptr<BaseEntity> current_entity,
        std::shared_ptr<MenuEntry> main_entry) {
    std::vector<std::shared_ptr<MenuEntry>> ret_vec;

    auto color = current_entity->renderableEntity_ ? sf::Color::Green : sf::Color::Red;

    auto ent1 = std::make_shared<MenuEntry>("Renderable");
    ent1->setColor(color);
    ret_vec.push_back(ent1);

    auto ent2 = std::make_shared<MenuEntry>(
            "Enable/Disable",
            std::optional<Command::Commands>(Command::Commands::TOGGLE_RENDERABLE));
    ent2->setColor(color);
    ent1->addEntry(ent2);
    ret_vec.push_back(ent2);

    main_entry->addEntry(ent1);

    return ret_vec;
}

std::vector<std::shared_ptr<MenuEntry>> makeCollisionEntry(
        std::shared_ptr<BaseEntity> current_entity,
        std::shared_ptr<MenuEntry> main_entry) {
    std::vector<std::shared_ptr<MenuEntry>> ret_vec;

    auto color = current_entity->collision_ ? sf::Color::Green : sf::Color::Red;

    auto ent1 = std::make_shared<MenuEntry>("Collision");
    ent1->setColor(color);
    ret_vec.push_back(ent1);

    auto ent2 = std::make_shared<MenuEntry>(
            "Enable/Disable",
            std::optional<Command::Commands>(Command::Commands::TOGGLE_COLLISION));
    ent2->setColor(color);
    ent1->addEntry(ent2);
    ret_vec.push_back(ent2);

    main_entry->addEntry(ent1);

    return ret_vec;
}

} // namespace


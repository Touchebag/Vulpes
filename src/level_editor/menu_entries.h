
namespace menu_entries {

std::shared_ptr<MenuEntry> makeMenuEntry(
        std::string name,
        sf::Color color,
        std::optional<Command::Commands> command) {
    auto entry = std::make_shared<MenuEntry>(name, command);
    entry->setColor(color);

    return entry;
}

std::shared_ptr<MenuEntry> makeRenderableEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->getComponent<Rendering>() ? sf::Color::Green : sf::Color::Red;

    auto entry = makeMenuEntry("Renderable", color, std::nullopt);

    entry->addEntry(makeMenuEntry("Enable/Disable", color, {Command::Commands::TOGGLE_RENDERABLE}));

    if (current_entity->getComponent<Rendering>()) {
        entry->addEntry(makeMenuEntry("Sprite", color, {Command::Commands::RENDERABLE_SPRITE_CHANGE}));

        auto tiling_entry = makeMenuEntry("Tiling", color, std::nullopt);

        // TODO Enable
        // auto tile_x = current_entity->getComponent<Rendering>()->tiling_x_;
        // auto tile_y = current_entity->getComponent<Rendering>()->tiling_y_;

        auto tiling_color = sf::Color::Green;
        tiling_entry->addEntry(makeMenuEntry("Tile X", tiling_color, {Command::Commands::RENDERABLE_TILING_X}));
        tiling_entry->addEntry(makeMenuEntry("Tile Y", tiling_color, {Command::Commands::RENDERABLE_TILING_Y}));

        entry->addEntry(tiling_entry);
    }

    return entry;
}

std::shared_ptr<MenuEntry> makeCollisionEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->getComponent<Collision>() ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry("Collision", color, std::nullopt);

    entry->addEntry(makeMenuEntry("Enable/Disable", color, {Command::Commands::TOGGLE_COLLISION}));

    return entry;
}

std::shared_ptr<MenuEntry> makeMovableEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->getComponent<Movement>() ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry("Movement", color, std::nullopt);

    entry->addEntry(makeMenuEntry("Enable/Disable", color, {Command::Commands::TOGGLE_MOVABLE}));

    return entry;
}

std::shared_ptr<MenuEntry> makePhysicsEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->getComponent<Physics>() ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry("Physics", color, std::nullopt);

    entry->addEntry(makeMenuEntry("Enable/Disable", color, {Command::Commands::TOGGLE_PHYSICS}));

    return entry;
}

std::shared_ptr<MenuEntry> makeActionsEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->getComponent<Actions>() ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry("Actions", color, std::nullopt);

    entry->addEntry(makeMenuEntry("Enable/Disable", color, {Command::Commands::TOGGLE_ACTIONS}));

    return entry;
}

} // namespace


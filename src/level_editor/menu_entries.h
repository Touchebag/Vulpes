
namespace menu_entries {

std::shared_ptr<MenuEntry> makeMenuEntry(
        std::shared_ptr<BaseEntity> current_entity,
        std::string name,
        sf::Color color,
        std::optional<Command::Commands> command) {
    auto entry = std::make_shared<MenuEntry>(name, command);
    entry->setColor(color);

    return entry;
}

std::shared_ptr<MenuEntry> makeRenderableEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->renderableEntity_ ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry(current_entity, "Renderable", color, std::nullopt);

    entry->addEntry(makeMenuEntry(current_entity, "Enable/Disable", color, {Command::Commands::TOGGLE_RENDERABLE}));

    auto tiling_entry = makeMenuEntry(current_entity, "Tiling", color, std::nullopt);

    tiling_entry->addEntry(makeMenuEntry(current_entity, "No tiling", color, {Command::Commands::RENDERABLE_TILING_NONE}));
    tiling_entry->addEntry(makeMenuEntry(current_entity, "Tile X", color, {Command::Commands::RENDERABLE_TILING_X}));
    tiling_entry->addEntry(makeMenuEntry(current_entity, "Tile Y", color, {Command::Commands::RENDERABLE_TILING_Y}));
    tiling_entry->addEntry(makeMenuEntry(current_entity, "Tile X & Y", color, {Command::Commands::RENDERABLE_TILING_XY}));

    entry->addEntry(tiling_entry);

    return entry;
}

std::shared_ptr<MenuEntry> makeCollisionEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->collision_ ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry(current_entity, "Collision", color, std::nullopt);

    entry->addEntry(makeMenuEntry(current_entity, "Enable/Disable", color, {Command::Commands::TOGGLE_COLLISION}));

    return entry;
}

std::shared_ptr<MenuEntry> makeMovableEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->movableEntity_ ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry(current_entity, "Movable", color, std::nullopt);

    entry->addEntry(makeMenuEntry(current_entity, "Enable/Disable", color, {Command::Commands::TOGGLE_MOVABLE}));

    return entry;
}

std::shared_ptr<MenuEntry> makePhysicsEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->physics_ ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry(current_entity, "Physics", color, std::nullopt);

    entry->addEntry(makeMenuEntry(current_entity, "Enable/Disable", color, {Command::Commands::TOGGLE_PHYSICS}));

    return entry;
}

std::shared_ptr<MenuEntry> makeActionsEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->actions_ ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry(current_entity, "Actions", color, std::nullopt);

    entry->addEntry(makeMenuEntry(current_entity, "Enable/Disable", color, {Command::Commands::TOGGLE_ACTIONS}));

    return entry;
}

} // namespace


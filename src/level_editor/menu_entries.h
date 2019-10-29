
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
    auto color = current_entity->renderableEntity_ ? sf::Color::Green : sf::Color::Red;

    auto entry = makeMenuEntry("Renderable", color, std::nullopt);

    entry->addEntry(makeMenuEntry("Enable/Disable", color, {Command::Commands::TOGGLE_RENDERABLE}));

    if (current_entity->renderableEntity_) {
        entry->addEntry(makeMenuEntry("Sprite", color, {Command::Commands::RENDERABLE_SPRITE_CHANGE}));

        auto tiling_entry = makeMenuEntry("Tiling", color, std::nullopt);

        auto tile_x = current_entity->renderableEntity_->tiling_x_;
        auto tile_y = current_entity->renderableEntity_->tiling_y_;

        auto tiling_color = !tile_x && !tile_y ? sf::Color::Green : sf::Color::Red;
        tiling_entry->addEntry(makeMenuEntry("No tiling", tiling_color, {Command::Commands::RENDERABLE_TILING_NONE}));
        tiling_color = tile_x && !tile_y ? sf::Color::Green : sf::Color::Red;
        tiling_entry->addEntry(makeMenuEntry("Tile X", tiling_color, {Command::Commands::RENDERABLE_TILING_X}));
        tiling_color = !tile_x && tile_y ? sf::Color::Green : sf::Color::Red;
        tiling_entry->addEntry(makeMenuEntry("Tile Y", tiling_color, {Command::Commands::RENDERABLE_TILING_Y}));
        tiling_color = tile_x && tile_y ? sf::Color::Green : sf::Color::Red;
        tiling_entry->addEntry(makeMenuEntry("Tile X & Y", tiling_color, {Command::Commands::RENDERABLE_TILING_XY}));

        entry->addEntry(tiling_entry);

    }

    return entry;
}

std::shared_ptr<MenuEntry> makeCollisionEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->collision_ ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry("Collision", color, std::nullopt);

    entry->addEntry(makeMenuEntry("Enable/Disable", color, {Command::Commands::TOGGLE_COLLISION}));

    return entry;
}

std::shared_ptr<MenuEntry> makeMovableEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->movableEntity_ ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry("Movable", color, std::nullopt);

    entry->addEntry(makeMenuEntry("Enable/Disable", color, {Command::Commands::TOGGLE_MOVABLE}));

    return entry;
}

std::shared_ptr<MenuEntry> makePhysicsEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->physics_ ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry("Physics", color, std::nullopt);

    entry->addEntry(makeMenuEntry("Enable/Disable", color, {Command::Commands::TOGGLE_PHYSICS}));

    return entry;
}

std::shared_ptr<MenuEntry> makeActionsEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->actions_ ? sf::Color::Green : sf::Color::Red;
    auto entry = makeMenuEntry("Actions", color, std::nullopt);

    entry->addEntry(makeMenuEntry("Enable/Disable", color, {Command::Commands::TOGGLE_ACTIONS}));

    return entry;
}

} // namespace



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
    auto entity = makeMenuEntry(current_entity, "Renderable", color, std::nullopt);

    entity->addEntry(makeMenuEntry(current_entity, "Enable/Disable", color, {Command::Commands::TOGGLE_RENDERABLE}));

    return entity;
}

std::shared_ptr<MenuEntry> makeCollisionEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->collision_ ? sf::Color::Green : sf::Color::Red;
    auto entity = makeMenuEntry(current_entity, "Collision", color, std::nullopt);

    entity->addEntry(makeMenuEntry(current_entity, "Enable/Disable", color, {Command::Commands::TOGGLE_COLLISION}));

    return entity;
}

std::shared_ptr<MenuEntry> makeMovableEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->movableEntity_ ? sf::Color::Green : sf::Color::Red;
    auto entity = makeMenuEntry(current_entity, "Movable", color, std::nullopt);

    entity->addEntry(makeMenuEntry(current_entity, "Enable/Disable", color, {Command::Commands::TOGGLE_MOVABLE}));

    return entity;
}

std::shared_ptr<MenuEntry> makePhysicsEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->physics_ ? sf::Color::Green : sf::Color::Red;
    auto entity = makeMenuEntry(current_entity, "Physics", color, std::nullopt);

    entity->addEntry(makeMenuEntry(current_entity, "Enable/Disable", color, {Command::Commands::TOGGLE_PHYSICS}));

    return entity;
}

std::shared_ptr<MenuEntry> makeActionsEntry(std::shared_ptr<BaseEntity> current_entity) {
    auto color = current_entity->actions_ ? sf::Color::Green : sf::Color::Red;
    auto entity = makeMenuEntry(current_entity, "Actions", color, std::nullopt);

    entity->addEntry(makeMenuEntry(current_entity, "Enable/Disable", color, {Command::Commands::TOGGLE_ACTIONS}));

    return entity;
}

} // namespace


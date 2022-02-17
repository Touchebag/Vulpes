#include "command.h"

#include <memory>

#include "editor_loop/editor_environment.h"
#include "components/collision/collideables/movement/collideable_static.h"
#include "utils/log.h"

Command::Command(std::weak_ptr<EditorEnvironment> editor_env) :
        editor_env_(editor_env) {
    if (!editor_env.lock()) {
        LOGE("Command, failed to lock editor_env");
    }
}

void Command::update() {
    auto editor_env = editor_env_.lock();
    switch (editor_env->current_command) {
        case (Command::Commands::MOVE):
            {
                auto mouse_world_dist = editor_env->mouse->getMouseWorldDistance();

                if(auto transform = editor_env->current_entity->getComponent<Transform>()) {
                    transform->setPosition(
                            original_entity_position_.first + static_cast<int>(mouse_world_dist.first),
                            original_entity_position_.second + static_cast<int>(mouse_world_dist.second));
                }
            }
            break;
        default:
            break;
    }
}

void Command::add() {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    add(entity);
}

void Command::add(std::shared_ptr<BaseEntity> entity) {
    auto editor_env = editor_env_.lock();

    if (editor_env->current_entity) {
        return;
    }

    std::shared_ptr<Transform> trans = std::make_shared<Transform>(entity->components_);
    std::shared_ptr<CollideableStatic> coll = std::make_shared<CollideableStatic>(entity->components_);
    std::shared_ptr<Rendering> render = std::make_shared<Rendering>(entity->components_);

    coll->setHitbox(50, 50);
    render->setLayer(current_layer_);

    auto mouse_world_pos_ = editor_env->mouse->getMouseWorldPosition();
    trans->setPosition(static_cast<int>(mouse_world_pos_.first), static_cast<int>(mouse_world_pos_.second));
    entity->setComponent<Transform>(trans);

    render->setSize(50, 50);
    render->loadTexture("box.png", File());
    entity->setComponent<Rendering>(render);

    System::IWorldModify::addEntity(entity);
}

void Command::remove() {
    auto editor_env = editor_env_.lock();

    if (auto entity = editor_env->current_entity) {
        // Ensure weak_ptr in render expires
        entity->setComponent<Rendering>({});

        System::IWorldModify::removeEntity(entity);
        editor_env->current_entity = nullptr;
    }
}

void Command::copy() {
    auto editor_env = editor_env_.lock();

    if (auto entity = editor_env->current_entity) {
        editor_env->entity_clipboard = entity->outputToJson().value();
    }
}

void Command::paste() {
    auto editor_env = editor_env_.lock();

    // Only copy if no entity is currently selected
    if (editor_env->current_entity) {
        return;
    }

    auto cp_entity = BaseEntity::createFromJson(editor_env->entity_clipboard);

    auto mouse_world_pos = editor_env->mouse->getMouseWorldPosition();
    if(auto transform = cp_entity->getComponent<Transform>()) {
        transform->setPosition(static_cast<int>(mouse_world_pos.first), static_cast<int>(mouse_world_pos.second));
    }
    System::IWorldModify::addEntity(cp_entity);
}

void Command::handleCommand(Commands command) {
    auto editor_env = editor_env_.lock();

    if (!editor_env->current_entity) {
        return;
    }

    switch (command) {
        case (Commands::MOVE):
            {
                if (auto trans = editor_env->current_entity->getComponent<Transform>()) {
                    original_entity_position_ = {trans->getX(), trans->getY()};
                    editor_env->current_command = Command::Commands::MOVE;
                }

            }
            break;
        default:
            LOGW("Unknown command");
            break;
    }
}

void Command::stopCommand() {
    auto editor_env = editor_env_.lock();

    editor_env->current_command = Commands::NONE;
}

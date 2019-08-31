#include <memory>

#include "command.h"

#include "log.h"

#include "operations/add.h"
#include "operations/remove.h"
#include "operations/resize.h"
#include "operations/move.h"

Command::Command(std::shared_ptr<History> history,
        std::shared_ptr<Operation> current_operation,
        std::shared_ptr<Mouse> mouse) :
    history_(history),
    current_operation_(current_operation),
    mouse_(mouse),
    current_command_(Commands::NONE) {
}

void Command::update() {
    switch (current_command_) {
        case Commands::RESIZE:
            {
                auto mouse_world_dist = mouse_->getMouseWorldDistance();
                auto hbox = current_operation_->before_;

                current_entity_->setHitbox(static_cast<int>(static_cast<float>(hbox.first) + (mouse_world_dist.first * 2.0)), static_cast<int>(static_cast<float>(hbox.second) + (mouse_world_dist.second * 2.0)));
            }
            break;
        case (Command::Commands::MOVE):
            {
                auto mouse_world_dist = mouse_->getMouseWorldDistance();
                auto pos = current_operation_->before_;

                current_entity_->setPosition(static_cast<int>(static_cast<float>(pos.first) + mouse_world_dist.first), static_cast<int>(static_cast<float>(pos.second) + mouse_world_dist.second));
            }
        default:
            break;
    }
}

void Command::add() {
    std::shared_ptr<BaseEntity> entity = std::make_shared<BaseEntity>();
    add(entity);
}

void Command::add(std::shared_ptr<BaseEntity> entity) {
    std::shared_ptr<Hitbox> hitbox = std::make_shared<Hitbox>();
    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<RenderableEntity> render = std::make_shared<RenderableEntity>(trans);

    hitbox->setHitbox(50, 50);
    entity->hitbox_ = hitbox;

    auto mouse_world_pos_ = mouse_->getMouseWorldPosition();
    trans->setPosition(static_cast<int>(mouse_world_pos_.first), static_cast<int>(mouse_world_pos_.second));
    entity->trans_ = trans;

    render->loadTexture("box.png");
    entity->renderableEntity_ = render;

    World::getInstance().addEntity(entity, current_layer_);

    current_operation_ = std::make_shared<operation::Add>(operation::Add());
    current_operation_->entity_ = entity;
    current_operation_->layer_ = current_layer_;

    history_->addOperation(current_operation_);
}

void Command::remove(std::shared_ptr<BaseEntity> entity) {
    if (!entity) {
        return;
    }

    current_operation_ = std::make_shared<operation::Remove>(operation::Remove());
    current_operation_->entity_ = entity;
    current_operation_->layer_ = current_layer_;

    history_->addOperation(current_operation_);

    World::getInstance().removeEntity(entity, current_layer_);
    current_entity_ = nullptr;
}

void Command::copy(std::shared_ptr<BaseEntity> entity) {
    if (entity) {
        std::shared_ptr<BaseEntity> cp_entity = std::make_shared<BaseEntity>();
        cp_entity->loadFromJson(entity->outputToJson().value());

        auto mouse_world_pos = mouse_->getMouseWorldPosition();
        cp_entity->setPosition(static_cast<int>(mouse_world_pos.first), static_cast<int>(mouse_world_pos.second));
        World::getInstance().addEntity(cp_entity, current_layer_);

        current_operation_ = std::make_shared<operation::Add>(operation::Add());
        current_operation_->entity_ = cp_entity;
        current_operation_->layer_ = current_layer_;

        history_->addOperation(current_operation_);
    }
}

void Command::startCommand(Commands command) {
    if (!current_entity_) {
        return;
    }

    switch (command) {
        case (Commands::RESIZE) :
            {
                auto hbox = current_entity_->getHitbox();

                current_operation_ = std::make_shared<operation::Resize>(operation::Resize());
                current_operation_->entity_ = current_entity_;
                current_operation_->before_ = {hbox.width_, hbox.height_};

                current_command_ = Commands::RESIZE;
            }
            break;
        case (Commands::MOVE):
            {
                auto pos = current_entity_->getPosition();

                current_operation_ = std::make_shared<operation::Move>(operation::Move());
                current_operation_->entity_ = current_entity_;
                current_operation_->before_ = {pos.x, pos.y};

                current_command_ = Command::Commands::MOVE;
            }
            break;
        default:
            LOGW("Unknown command");
            break;
    }
}

void Command::stopCommand() {
    switch (current_command_) {
        case Command::Commands::RESIZE:
            {
                auto hbox = current_entity_->getHitbox();

                current_operation_->after_ = {hbox.width_, hbox.height_};

                history_->addOperation(current_operation_);
                break;
            }
        case Command::Commands::MOVE:
            {
                auto pos = current_entity_->getPosition();

                current_operation_->after_ = {pos.x, pos.y};

                history_->addOperation(current_operation_);
                break;
            }
        default:
            break;
    }
    current_command_ = Commands::NONE;
}

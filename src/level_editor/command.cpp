#include "command.h"

#include <memory>

#include "operation.h"
#include "log.h"

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

                if (current_operation_->before_) {
                    auto j = current_operation_->before_.value();
                    if (j.contains("Hitbox")) {
                        auto hbox = j["Hitbox"];
                        current_entity_->setHitbox(
                                static_cast<int>(static_cast<float>(hbox["width"].get<int>()) + (mouse_world_dist.first * 2.0)),
                                static_cast<int>(static_cast<float>(hbox["height"].get<int>()) + (mouse_world_dist.second * 2.0)));
                    }
                }
            }
            break;
        case (Command::Commands::MOVE):
            {
                auto mouse_world_dist = mouse_->getMouseWorldDistance();

                if (current_operation_->before_) {
                    auto j = current_operation_->before_.value();
                    if (j.contains("Transform")) {
                        auto trans = j["Transform"];
                        current_entity_->setPosition(
                                static_cast<int>(static_cast<float>(trans["pos_x"]) + mouse_world_dist.first),
                                static_cast<int>(static_cast<float>(trans["pos_y"]) + mouse_world_dist.second));
                    }
                }
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

    current_operation_ = std::make_shared<Operation>();
    current_operation_->entity_ = entity;
    current_operation_->layer_ = current_layer_;

    history_->addOperation(current_operation_);
}

void Command::remove(std::shared_ptr<BaseEntity> entity) {
    if (!entity) {
        return;
    }

    current_operation_ = std::make_shared<Operation>();
    current_operation_->entity_ = entity;
    current_operation_->layer_ = current_layer_;
    current_operation_->before_ = entity->outputToJson();

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

        current_operation_ = std::make_shared<Operation>();
        current_operation_->entity_ = cp_entity;
        current_operation_->layer_ = current_layer_;

        history_->addOperation(current_operation_);
    }
}

// TODO No longer only start command. Also handling. Rename
void Command::startCommand(Commands command) {
    if (!current_entity_) {
        return;
    }

    switch (command) {
        case (Commands::RESIZE) :
            {
                current_operation_ = std::make_shared<Operation>();
                current_operation_->entity_ = current_entity_;
                current_operation_->before_ = current_entity_->outputToJson();
                current_operation_->layer_ = current_layer_;

                current_command_ = Commands::RESIZE;
            }
            break;
        case (Commands::MOVE):
            {
                current_operation_ = std::make_shared<Operation>();
                current_operation_->entity_ = current_entity_;
                current_operation_->before_ = current_entity_->outputToJson();
                current_operation_->layer_ = current_layer_;

                current_command_ = Command::Commands::MOVE;
            }
            break;
        case (Commands::TOGGLE_RENDERABLE):
            {
                current_operation_ = std::make_shared<Operation>();
                current_operation_->entity_ = current_entity_;
                current_operation_->before_ = current_entity_->outputToJson();
                current_operation_->layer_ = current_layer_;

                World::getInstance().removeEntity(current_entity_, World::Layer::MAIN);
                if (current_entity_->renderableEntity_) {
                    current_entity_->renderableEntity_ = {};
                } else {
                    auto renderable = std::make_shared<RenderableEntity>(current_entity_->trans_);
                    renderable->loadTexture("box.png");
                    current_entity_->renderableEntity_ = renderable;
                }
                World::getInstance().addEntity(current_entity_, World::Layer::MAIN);
                current_operation_->after_ = current_entity_->outputToJson();

                history_->addOperation(current_operation_);
                break;
            }
        case (Commands::TOGGLE_COLLISION):
            {
                current_operation_ = std::make_shared<Operation>();
                current_operation_->entity_ = current_entity_;
                current_operation_->before_ = current_entity_->outputToJson();
                current_operation_->layer_ = current_layer_;

                World::getInstance().removeEntity(current_entity_, World::Layer::MAIN);
                if (current_entity_->collision_) {
                    current_entity_->collision_ = {};
                } else {
                    auto collision = std::make_shared<Collision>(current_entity_->trans_, current_entity_->hitbox_);
                    current_entity_->collision_ = collision;
                }
                World::getInstance().addEntity(current_entity_, World::Layer::MAIN);
                current_operation_->after_ = current_entity_->outputToJson();

                history_->addOperation(current_operation_);
                break;
            }
        case (Commands::TOGGLE_MOVABLE):
            {
                current_operation_ = std::make_shared<Operation>();
                current_operation_->entity_ = current_entity_;
                current_operation_->before_ = current_entity_->outputToJson();
                current_operation_->layer_ = current_layer_;

                World::getInstance().removeEntity(current_entity_, World::Layer::MAIN);
                if (current_entity_->movableEntity_) {
                    current_entity_->movableEntity_ = {};
                } else {
                    auto movable = std::make_shared<MovableEntity>(current_entity_->trans_, current_entity_->hitbox_, current_entity_->collision_);
                    current_entity_->movableEntity_ = movable;
                }
                World::getInstance().addEntity(current_entity_, World::Layer::MAIN);
                current_operation_->after_ = current_entity_->outputToJson();

                history_->addOperation(current_operation_);
                break;
            }
        case (Commands::TOGGLE_PHYSICS):
            {
                current_operation_ = std::make_shared<Operation>();
                current_operation_->entity_ = current_entity_;
                current_operation_->before_ = current_entity_->outputToJson();
                current_operation_->layer_ = current_layer_;

                World::getInstance().removeEntity(current_entity_, World::Layer::MAIN);
                if (current_entity_->physics_) {
                    current_entity_->physics_ = {};
                } else {
                    auto physics = std::make_shared<Physics>(
                            current_entity_->statefulEntity_,
                            current_entity_->renderableEntity_,
                            current_entity_->movableEntity_,
                            current_entity_->animatedEntity_,
                            current_entity_->actions_,
                            current_entity_->collision_);
                    current_entity_->physics_ = physics;
                }
                World::getInstance().addEntity(current_entity_, World::Layer::MAIN);
                current_operation_->after_ = current_entity_->outputToJson();

                history_->addOperation(current_operation_);
                break;
            }
        case (Commands::TOGGLE_ACTIONS):
            {
                current_operation_ = std::make_shared<Operation>();
                current_operation_->entity_ = current_entity_;
                current_operation_->before_ = current_entity_->outputToJson();
                current_operation_->layer_ = current_layer_;

                World::getInstance().removeEntity(current_entity_, World::Layer::MAIN);
                if (current_entity_->actions_) {
                    current_entity_->actions_ = {};
                } else {
                    auto actions = std::make_shared<Actions>();
                    current_entity_->actions_ = actions;
                }
                World::getInstance().addEntity(current_entity_, World::Layer::MAIN);
                current_operation_->after_ = current_entity_->outputToJson();

                history_->addOperation(current_operation_);
                break;
            }
        default:
            LOGW("Unknown command");
            break;
    }
}

void Command::stopCommand() {
    switch (current_command_) {
        case Command::Commands::RESIZE:
            {
                current_operation_->after_ = current_entity_->outputToJson();

                history_->addOperation(current_operation_);
                break;
            }
        case Command::Commands::MOVE:
            {
                current_operation_->after_ = current_entity_->outputToJson();

                history_->addOperation(current_operation_);
                break;
            }
        default:
            break;
    }
    current_command_ = Commands::NONE;
}

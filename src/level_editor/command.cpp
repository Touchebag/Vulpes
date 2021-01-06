#include "command.h"

#include <memory>

#include "operation.h"
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
        case Commands::RESIZE:
            {
                auto mouse_world_dist = editor_env->mouse->getMouseWorldDistance();

                if (editor_env->current_operation->before_) {
                    auto j = editor_env->current_operation->before_.value();
                    if (j.contains("Collision")) {
                        auto coll = j["Collision"];
                        if (editor_env->current_entity->collision_) {
                            editor_env->current_entity->collision_->setHitbox(
                                    static_cast<int>(static_cast<float>(coll["width"].get<int>()) + (mouse_world_dist.first * 2.0)),
                                    static_cast<int>(static_cast<float>(coll["height"].get<int>()) + (mouse_world_dist.second * 2.0)));
                        }
                    }
                    if (j.contains("Renderable")) {
                        auto render = j["Renderable"];
                        if (editor_env->current_entity->renderableEntity_) {
                            int w = 0;
                            int h = 0;
                            if (render.contains("width")) {
                                w = static_cast<int>(static_cast<float>(render["width"].get<int>()) + (mouse_world_dist.first * 2.0));
                            }
                            if (render.contains("height")) {
                                h = static_cast<int>(static_cast<float>(render["height"].get<int>()) + (mouse_world_dist.second * 2.0));
                            }
                            editor_env->current_entity->renderableEntity_->setSize(w, h);
                        }
                    }
                }
            }
            break;
        case (Command::Commands::MOVE):
            {
                auto mouse_world_dist = editor_env->mouse->getMouseWorldDistance();

                if (editor_env->current_operation->before_) {
                    auto j = editor_env->current_operation->before_.value();
                    if (j.contains("Transform")) {
                        if(auto transform = editor_env->current_entity->trans_) {
                            auto trans = j["Transform"];
                            transform->setPosition(
                                    static_cast<int>(static_cast<float>(trans["pos_x"]) + mouse_world_dist.first),
                                    static_cast<int>(static_cast<float>(trans["pos_y"]) + mouse_world_dist.second));
                        }
                    }
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

    std::shared_ptr<Transform> trans = std::make_shared<Transform>();
    std::shared_ptr<CollisionStatic> coll = std::make_shared<CollisionStatic>(trans);
    std::shared_ptr<RenderableEntity> render = std::make_shared<RenderableEntity>(trans, std::weak_ptr<MovableEntity>());

    coll->setHitbox(50, 50);
    render->setLayer(current_layer_);

    auto mouse_world_pos_ = editor_env->mouse->getMouseWorldPosition();
    trans->setPosition(static_cast<int>(mouse_world_pos_.first), static_cast<int>(mouse_world_pos_.second));
    entity->trans_ = trans;

    render->setSize(50, 50);
    render->loadTexture("box.png", File());
    entity->renderableEntity_ = render;

    World::getInstance<World::IWorldModify>().addEntity(entity);

    editor_env->current_operation = std::make_shared<Operation>();
    editor_env->current_operation->entity_ = entity;
    editor_env->current_operation->after_ = entity->outputToJson();

    editor_env->history->addOperation(editor_env->current_operation);
}

void Command::remove(std::shared_ptr<BaseEntity> entity) {
    if (!entity) {
        return;
    }

    auto editor_env = editor_env_.lock();

    editor_env->current_operation = std::make_shared<Operation>();
    editor_env->current_operation->entity_ = entity;
    editor_env->current_operation->before_ = entity->outputToJson();

    // Ensure weak_ptr in render expires
    entity->renderableEntity_.reset();

    editor_env->history->addOperation(editor_env->current_operation);

    World::getInstance<World::IWorldModify>().removeEntity(entity);
    editor_env->current_entity = nullptr;
}

void Command::copy(std::shared_ptr<BaseEntity> entity) {
    if (entity) {
        auto editor_env = editor_env_.lock();

        auto cp_entity = BaseEntity::createFromJson(entity->outputToJson().value());

        auto mouse_world_pos = editor_env->mouse->getMouseWorldPosition();
        if(auto transform = cp_entity->trans_) {
            transform->setPosition(static_cast<int>(mouse_world_pos.first), static_cast<int>(mouse_world_pos.second));
        }
        World::getInstance<World::IWorldModify>().addEntity(cp_entity);

        editor_env->current_operation = std::make_shared<Operation>();
        editor_env->current_operation->entity_ = cp_entity;
        editor_env->current_operation->after_ = cp_entity->outputToJson();

        editor_env->history->addOperation(editor_env->current_operation);
    }
}

void Command::handleCommand(Commands command) {
    auto editor_env = editor_env_.lock();

    if (!editor_env->current_entity) {
        return;
    }

    switch (command) {
        case (Commands::RESIZE) :
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                editor_env->current_command = Commands::RESIZE;
            }
            break;
        case (Commands::MOVE):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                editor_env->current_command = Command::Commands::MOVE;
            }
            break;
        case (Commands::TOGGLE_RENDERABLE):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                World::getInstance<World::IWorldModify>().removeEntity(editor_env->current_entity);
                if (editor_env->current_entity->renderableEntity_) {
                    editor_env->current_entity->renderableEntity_ = {};
                } else {
                    auto renderable = std::make_shared<RenderableEntity>(editor_env->current_entity->trans_,
                                                                         editor_env->current_entity->movableEntity_);
                    renderable->loadTexture("box.png", File());
                    editor_env->current_entity->renderableEntity_ = renderable;
                }
                World::getInstance<World::IWorldModify>().addEntity(editor_env->current_entity);
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case (Commands::TOGGLE_COLLISION):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                World::getInstance<World::IWorldModify>().removeEntity(editor_env->current_entity);
                if (editor_env->current_entity->collision_) {
                    editor_env->current_entity->collision_ = {};
                } else {
                    auto collision = std::make_shared<CollisionStatic>(editor_env->current_entity->trans_);
                    editor_env->current_entity->collision_ = collision;
                }
                World::getInstance<World::IWorldModify>().addEntity(editor_env->current_entity);
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case (Commands::TOGGLE_MOVABLE):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                World::getInstance<World::IWorldModify>().removeEntity(editor_env->current_entity);
                if (editor_env->current_entity->movableEntity_) {
                    editor_env->current_entity->movableEntity_ = {};
                } else {
                    auto movable = std::make_shared<MovableEntity>(editor_env->current_entity->trans_, editor_env->current_entity->collision_);
                    editor_env->current_entity->movableEntity_ = movable;
                }
                World::getInstance<World::IWorldModify>().addEntity(editor_env->current_entity);
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case (Commands::TOGGLE_PHYSICS):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                World::getInstance<World::IWorldModify>().removeEntity(editor_env->current_entity);
                if (editor_env->current_entity->physics_) {
                    editor_env->current_entity->physics_ = {};
                } else {
                    auto physics = std::make_shared<Physics>(
                            editor_env->current_entity->statefulEntity_,
                            editor_env->current_entity->movableEntity_,
                            editor_env->current_entity->animatedEntity_,
                            editor_env->current_entity->actions_);
                    editor_env->current_entity->physics_ = physics;
                }
                World::getInstance<World::IWorldModify>().addEntity(editor_env->current_entity);
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case (Commands::TOGGLE_ACTIONS):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                World::getInstance<World::IWorldModify>().removeEntity(editor_env->current_entity);
                if (editor_env->current_entity->actions_) {
                    editor_env->current_entity->actions_ = {};
                } else {
                    auto actions = std::make_shared<Actions>(editor_env->current_entity->death_);
                    editor_env->current_entity->actions_ = actions;
                }
                World::getInstance<World::IWorldModify>().addEntity(editor_env->current_entity);
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case (Commands::RENDERABLE_SPRITE_CHANGE):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                text_input_ = std::make_shared<TextInput>();

                editor_env->current_command = Command::Commands::RENDERABLE_SPRITE_CHANGE;
                break;
            }
        case (Commands::RENDERABLE_TILING_NONE):
            if (editor_env->current_entity->renderableEntity_) {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                editor_env->current_entity->renderableEntity_->setTiling(false, false);

                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();
                editor_env->history->addOperation(editor_env->current_operation);
            }
            break;
        case (Commands::RENDERABLE_TILING_X):
            if (editor_env->current_entity->renderableEntity_) {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                editor_env->current_entity->renderableEntity_->setTiling(true, false);

                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();
                editor_env->history->addOperation(editor_env->current_operation);
            }
            break;
        case (Commands::RENDERABLE_TILING_Y):
            if (editor_env->current_entity->renderableEntity_) {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                editor_env->current_entity->renderableEntity_->setTiling(false, true);

                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();
                editor_env->history->addOperation(editor_env->current_operation);
            }
            break;
        case (Commands::RENDERABLE_TILING_XY):
            if (editor_env->current_entity->renderableEntity_) {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                editor_env->current_entity->renderableEntity_->setTiling(true, true);

                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();
                editor_env->history->addOperation(editor_env->current_operation);
            }
            break;

        default:
            LOGW("Unknown command");
            break;
    }
}

void Command::stopCommand() {
    auto editor_env = editor_env_.lock();

    switch (editor_env->current_command) {
        case Command::Commands::RESIZE:
            {
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case Command::Commands::MOVE:
            {
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case (Commands::RENDERABLE_SPRITE_CHANGE):
            if (editor_env->current_entity->renderableEntity_ && text_input_) {
                try {
                    auto texture_name = text_input_->getString();
                    editor_env->current_entity->renderableEntity_->loadTexture(texture_name, File());

                    editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                    editor_env->history->addOperation(editor_env->current_operation);
                } catch (std::invalid_argument& e) {
                    LOGW("Unable to parse input text");
                    editor_env->current_operation.reset();
                }
            }
            break;
        default:
            break;
    }
    editor_env->current_command = Commands::NONE;
}

void Command::closeTextInput() {
    stopCommand();
    text_input_.reset();
}

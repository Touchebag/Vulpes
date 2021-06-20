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
                        if (editor_env->current_entity->getComponent<Collision>()) {
                            auto new_width = 0;
                            auto new_height = 0;

                            // If result would be negative, don't change
                            if (mouse_world_dist.first > 0
                               || (abs(mouse_world_dist.first) < coll["width"].get<float>() / 2.0)) {
                                new_width = static_cast<int>(coll["width"].get<float>() + (mouse_world_dist.first * 2.0));
                            }
                            if (mouse_world_dist.second > 0
                               || (abs(mouse_world_dist.second) < coll["height"].get<float>() / 2.0)) {
                                new_height = static_cast<int>(coll["height"].get<float>() + (mouse_world_dist.second * 2.0));
                            }

                            editor_env->current_entity->getComponent<Collision>()->getCollideable()->setHitbox(new_width, new_height);
                        }
                    }
                    if (j.contains("Renderable")) {
                        auto render = j["Renderable"];
                        if (editor_env->current_entity->getComponent<Rendering>()) {
                            int w = 0;
                            int h = 0;
                            if (render.contains("width")) {
                                w = static_cast<int>(static_cast<float>(render["width"].get<int>()) + (mouse_world_dist.first * 2.0));
                            }
                            if (render.contains("height")) {
                                h = static_cast<int>(static_cast<float>(render["height"].get<int>()) + (mouse_world_dist.second * 2.0));
                            }
                            editor_env->current_entity->getComponent<Rendering>()->setSize(w, h);
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
                        if(auto transform = editor_env->current_entity->getComponent<Transform>()) {
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
    entity->getComponent<Rendering>().reset();

    editor_env->history->addOperation(editor_env->current_operation);

    System::IWorldModify::removeEntity(entity);
    editor_env->current_entity = nullptr;
}

void Command::copy(std::shared_ptr<BaseEntity> entity) {
    if (entity) {
        auto editor_env = editor_env_.lock();

        auto cp_entity = BaseEntity::createFromJson(entity->outputToJson().value());

        auto mouse_world_pos = editor_env->mouse->getMouseWorldPosition();
        if(auto transform = cp_entity->getComponent<Transform>()) {
            transform->setPosition(static_cast<int>(mouse_world_pos.first), static_cast<int>(mouse_world_pos.second));
        }
        System::IWorldModify::addEntity(cp_entity);

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

                System::IWorldModify::removeEntity(editor_env->current_entity);
                if (editor_env->current_entity->getComponent<Rendering>()) {
                    editor_env->current_entity->setComponent<Rendering>({});
                } else {
                    auto renderable = std::make_shared<Rendering>(editor_env->current_entity->components_);
                    renderable->loadTexture("box.png", File());
                    editor_env->current_entity->setComponent<Rendering>(renderable);
                }
                System::IWorldModify::addEntity(editor_env->current_entity);
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case (Commands::TOGGLE_COLLISION):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                System::IWorldModify::removeEntity(editor_env->current_entity);
                if (editor_env->current_entity->getComponent<Collision>()) {
                    editor_env->current_entity->setComponent<Collision>({});
                } else {
                    editor_env->current_entity->setComponent<Collision>(std::make_shared<Collision>(editor_env->current_entity->components_));
                }
                System::IWorldModify::addEntity(editor_env->current_entity);
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case (Commands::TOGGLE_MOVABLE):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                System::IWorldModify::removeEntity(editor_env->current_entity);
                if (editor_env->current_entity->getComponent<Movement>()) {
                    editor_env->current_entity->setComponent<Movement>({});
                } else {
                    auto movable = std::make_shared<Movement>(editor_env->current_entity->components_);
                    editor_env->current_entity->setComponent<Movement>(movable);
                }
                System::IWorldModify::addEntity(editor_env->current_entity);
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case (Commands::TOGGLE_PHYSICS):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                System::IWorldModify::removeEntity(editor_env->current_entity);
                if (editor_env->current_entity->getComponent<Physics>()) {
                    editor_env->current_entity->setComponent<Physics>({});
                } else {
                    auto physics = std::make_shared<Physics>(editor_env->current_entity->components_);
                    editor_env->current_entity->setComponent<Physics>(physics);
                }
                System::IWorldModify::addEntity(editor_env->current_entity);
                editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                editor_env->history->addOperation(editor_env->current_operation);
                break;
            }
        case (Commands::TOGGLE_ACTIONS):
            {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                System::IWorldModify::removeEntity(editor_env->current_entity);
                if (editor_env->current_entity->getComponent<Actions>()) {
                    editor_env->current_entity->setComponent<Actions>({});
                } else {
                    auto actions = std::make_shared<Actions>(editor_env->current_entity->components_);
                    editor_env->current_entity->setComponent<Actions>(actions);
                }
                System::IWorldModify::addEntity(editor_env->current_entity);
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
        case (Commands::RENDERABLE_TILING_X):
            if (editor_env->current_entity->getComponent<Rendering>()) {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                text_input_ = std::make_shared<TextInput>();

                editor_env->current_command = Command::Commands::RENDERABLE_TILING_X;
            }
            break;
        case (Commands::RENDERABLE_TILING_Y):
            if (editor_env->current_entity->getComponent<Rendering>()) {
                editor_env->current_operation = std::make_shared<Operation>();
                editor_env->current_operation->entity_ = editor_env->current_entity;
                editor_env->current_operation->before_ = editor_env->current_entity->outputToJson();

                text_input_ = std::make_shared<TextInput>();

                editor_env->current_command = Command::Commands::RENDERABLE_TILING_Y;
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
            if (editor_env->current_entity->getComponent<Rendering>() && text_input_) {
                try {
                    auto texture_name = text_input_->getString();
                    editor_env->current_entity->getComponent<Rendering>()->loadTexture(texture_name, File());

                    editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                    editor_env->history->addOperation(editor_env->current_operation);
                } catch (std::invalid_argument& e) {
                    LOGW("Unable to parse input text");
                    editor_env->current_operation.reset();
                }
            }
            break;
        case (Commands::RENDERABLE_TILING_X):
            if (editor_env->current_entity->getComponent<Rendering>() && text_input_) {
                try {
                    auto tiling = stoi(text_input_->getString());
                    auto current_tiling = editor_env->current_entity->getComponent<Rendering>()->getTiling();
                    editor_env->current_entity->getComponent<Rendering>()->setTiling(tiling, current_tiling.second);

                    editor_env->current_operation->after_ = editor_env->current_entity->outputToJson();

                    editor_env->history->addOperation(editor_env->current_operation);
                } catch (std::invalid_argument& e) {
                    LOGW("Unable to parse input text");
                    editor_env->current_operation.reset();
                }
            }
            break;
        case (Commands::RENDERABLE_TILING_Y):
            if (editor_env->current_entity->getComponent<Rendering>() && text_input_) {
                try {
                    auto tiling = stoi(text_input_->getString());
                    auto current_tiling = editor_env->current_entity->getComponent<Rendering>()->getTiling();
                    editor_env->current_entity->getComponent<Rendering>()->setTiling(current_tiling.first, tiling);

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

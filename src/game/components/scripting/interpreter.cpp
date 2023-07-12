#include "interpreter.h"

#include <stack>

#include "system/system.h"
#include "components/collision/collideables/collideable_sensor.h"

#include "utils/log.h"

using scripting::Bool;
using scripting::Target;
using scripting::CollideableProperty;

// These are just for easy access
#define POP() pop_with_stack(stack)
#define PUSH(x) stack.push(x)

#define GET_TARGET \
    std::shared_ptr<ComponentStore> target; \
    switch (POP()) { \
        case Target::THIS: \
            target = extra_input.this_components; \
            break; \
        case Target::PLAYER: \
            target = player_components; \
            break; \
        default: \
            LOGE("Interpreter: Invalid target"); \
            exit(EXIT_FAILURE);\
            break; \
    }

namespace {

int pop_with_stack(std::stack<int>& stack) {
    if (stack.empty()) {
        // This should be handled by type checker
        // TODO Consider removing
        LOGW("SCRIPT: Stack is empty");
        return 0;
    }

    int ret = stack.top();
    stack.pop();

    return ret;
}

} // namespace

int Interpreter::executeProgram(Program program, ExtraInputData extra_input) {
    std::stack<int> stack;

    std::shared_ptr<ComponentStore> player_components = std::make_shared<ComponentStore>();
    if (auto player = System::IWorldModify::getPlayer().lock()) {
        player_components = player->components_;
    }

    auto byte_code = program.getProgram();

    for (auto pc = byte_code.begin(); pc < byte_code.end(); pc++) {
        LOGV("SCRIPT COMMAND START");
        switch (static_cast<scripting::Instruction>(*pc)) {
            case scripting::Instruction::INT:
                LOGV("INT");
                pc++;

                LOGV("%i", *pc);

                PUSH(*pc);
                break;
            case scripting::Instruction::BOOL:
                LOGV("BOOL");
                pc++;
                PUSH(*pc);
                break;
            case scripting::Instruction::STRING:
                LOGV("STRING");
                pc++;

                LOGV("%i", *pc);

                PUSH(*pc);
                break;
            case scripting::Instruction::FLOAT:
                LOGV("FLOAT");
                pc++;

                LOGV("%i", *pc);

                PUSH(*pc);
                break;
            case scripting::Instruction::GET:
            {
                LOGV("GET");

                auto var_name = program.getString(POP());
                auto value = extra_input.variables->at(var_name);

                PUSH(value);
                break;
            }
            case scripting::Instruction::SET:
            {
                LOGV("SET");

                auto value = POP();
                auto var_name = program.getString(POP());

                extra_input.variables->insert_or_assign(var_name, value);

                PUSH(0);
                break;
            }
            case scripting::Instruction::INC_VAR:
            {
                LOGV("INC_VAR");

                auto var_name = program.getString(POP());
                auto value = extra_input.variables->at(var_name);

                value++;

                extra_input.variables->insert_or_assign(var_name, value);

                PUSH(0);
                break;
            }
            case scripting::Instruction::DEC_VAR:
            {
                LOGV("DEC_VAR");

                auto var_name = program.getString(POP());
                auto value = extra_input.variables->at(var_name);

                value--;

                extra_input.variables->insert_or_assign(var_name, value);

                PUSH(0);
                break;
            }
            case scripting::Instruction::PLAYER:
                LOGV("PLAYER");
                PUSH(Target::PLAYER);
                break;
            case scripting::Instruction::THIS:
                LOGV("THIS");
                PUSH(Target::THIS);
                break;
            case scripting::Instruction::TOP_EDGE:
                LOGV("TOP_EDGE");
                PUSH(CollideableProperty::TOP_EDGE);
                break;
            case scripting::Instruction::BOTTOM_EDGE:
                LOGV("BOTTOM_EDGE");
                PUSH(CollideableProperty::BOTTOM_EDGE);
                break;
            case scripting::Instruction::LEFT_EDGE:
                LOGV("LEFT_EDGE");
                PUSH(CollideableProperty::LEFT_EDGE);
                break;
            case scripting::Instruction::RIGHT_EDGE:
                LOGV("RIGHT_EDGE");
                PUSH(CollideableProperty::RIGHT_EDGE);
                break;
            case scripting::Instruction::ADD:
            {
                LOGV("ADD");

                // Reverse stack order
                int i2 = POP();
                int i1 = POP();

                PUSH(i1 + i2);
                break;
            }
            case scripting::Instruction::SUB:
            {
                LOGV("SUB");

                // Reverse stack order
                int i2 = POP();
                int i1 = POP();

                PUSH(i1 - i2);
                break;
            }
            case scripting::Instruction::POSITION_X:
            {
                LOGV("POSITION_X");

                GET_TARGET;
                if (target) {
                    if (auto trans = target->getComponent<Transform>()) {
                        PUSH(trans->getX());
                        break;
                    } else {
                        LOGW("SCRIPT POSITION_X: Missing Transform component");
                    }
                }

                PUSH(0);
                break;
            }
            case scripting::Instruction::POSITION_Y:
            {
                LOGV("POSITION_Y");

                GET_TARGET;
                if (target) {
                    if (auto trans = target->getComponent<Transform>()) {
                        PUSH(trans->getY());
                        break;
                    } else {
                        LOGW("SCRIPT POSITION_Y: Missing Transform component");
                    }
                }

                PUSH(0);
                break;
            }
            case scripting::Instruction::COLLISION_HISTORY:
            {
                LOGV("COLLISION_HISTORY");

                auto target_prop = POP();
                auto history_index = POP();
                auto sensor_name = program.getString(POP());

                if (auto coll = extra_input.this_components->getComponent<Collision>()) {
                    if (std::shared_ptr<CollideableSensor> sens = coll->getSensor(sensor_name)) {
                        if (auto last_coll = sens->getLastCollideable(/* history_index */)) {
                            auto hbox = last_coll->getHitbox();
                            auto trans = last_coll->getTransform().lock();

                            if (!trans) {
                                LOGW("SCRIPT COLLISION_HISTORY: Could not lock transform");
                                break;
                            }

                            switch (target_prop) {
                                case CollideableProperty::TOP_EDGE:
                                    PUSH(trans->getY() - (hbox->height_ / 2));
                                    break;
                                case CollideableProperty::BOTTOM_EDGE:
                                    PUSH(trans->getY() + (hbox->height_ / 2));
                                    break;
                                case CollideableProperty::LEFT_EDGE:
                                    PUSH(trans->getX() - (hbox->width_ / 2));
                                    break;
                                case CollideableProperty::RIGHT_EDGE:
                                    PUSH(trans->getX() + (hbox->width_ / 2));
                                    break;
                                default:
                                    LOGW("SCRIPT COLLISION_HISTORY: This should never happen");
                                    break;
                            }
                        } else {
                            LOGW("SCRIPT COLLISION_HISTORY: Sensor '%s', history position %i not found", sensor_name.c_str(), history_index);
                        }
                    } else {
                        LOGW("SCRIPT COLLISION_HISTORY: Sensor '%s' not found", sensor_name.c_str());
                    }
                }

                break;
            }
            case scripting::Instruction::FRAME_TIMER:
                LOGV("FRAME_TIMER");
                if (static_cast<int>(extra_input.frame_timer) >= POP()) {
                    PUSH(Bool::TRUE);
                } else {
                    PUSH(Bool::FALSE);
                }
                break;
            case scripting::Instruction::COLLIDES:
                LOGV("COLLIDES");
                if (POP() == scripting::Target::PLAYER) {
                    if (auto this_coll = extra_input.this_components->getComponent<Collision>()) {
                        if (this_coll->collides(player_components->getComponent<Collision>())) {
                            PUSH(Bool::TRUE);
                        } else {
                            PUSH(Bool::FALSE);
                        }
                    } else {
                        LOGW("SCRIPT COLLIDES: Missing Collision component");
                        PUSH(Bool::FALSE);
                    }
                } else {
                    PUSH(Bool::FALSE);
                }
                break;
            case scripting::Instruction::FLAG:
            {
                LOGV("FLAG");

                auto str = program.getString(POP());
                LOGV("%s", str.c_str());

                if (System::getEnvironment()->getFlag(str)) {
                    PUSH(Bool::TRUE);
                } else {
                    PUSH(Bool::FALSE);
                }

                break;
            }
            case scripting::Instruction::ANIMATION_LOOPED:
                LOGV("ANIMATION_LOOPED");
                if (auto anim = extra_input.this_components->getComponent<Animation>()) {
                    if (anim->hasAnimationLooped()) {
                        PUSH(Bool::TRUE);
                    } else {
                        PUSH(Bool::FALSE);
                    }
                } else {
                    LOGW("SCRIPT ANIMATION_LOOPED: Missing Animation component");
                    PUSH(Bool::FALSE);
                }
                break;
            case scripting::Instruction::SENSOR:
            {
                LOGV("SENSOR");
                auto str = program.getString(POP());
                LOGV("%s", str.c_str());

                if (auto coll = extra_input.this_components->getComponent<Collision>()) {
                    if (coll->isSensorTriggered(str)) {
                        PUSH(Bool::TRUE);
                    } else {
                        PUSH(Bool::FALSE);
                    }
                } else {
                    LOGW("SCRIPT SENSOR: Missing Collision component");
                    PUSH(Bool::FALSE);
                }
                break;
            }
            case scripting::Instruction::GRT:
            {
                LOGV("GRT");
                auto a = POP();
                auto b = POP();

                LOGV("%i", a);
                LOGV("%i", b);

                // Popped in reverse order
                PUSH(b > a ? Bool::TRUE : Bool::FALSE);
                break;
            }
            case scripting::Instruction::LSS:
            {
                LOGV("LSS");
                auto a = POP();
                auto b = POP();

                LOGV("%i", a);
                LOGV("%i", b);

                PUSH(b < a ? Bool::TRUE : Bool::FALSE);
                break;
            }
            case scripting::Instruction::NOT:
            {
                LOGV("NOT");
                auto a = POP();

                PUSH(a ? Bool::FALSE : Bool::TRUE);
                break;
            }
            case scripting::Instruction::AND:
            {
                LOGV("AND");
                auto a = POP();
                auto b = POP();

                LOGV("%i", a);
                LOGV("%i", b);

                PUSH(a && b ? Bool::TRUE : Bool::FALSE);
                break;
            }
            case scripting::Instruction::OR:
            {
                LOGV("OR");
                auto a = POP();
                auto b = POP();

                LOGV("%i", a);
                LOGV("%i", b);

                PUSH(a || b ? Bool::TRUE : Bool::FALSE);
                break;
            }
            case scripting::Instruction::ACTION:
                LOGV("ACTION");

                if (auto act = extra_input.this_components->getComponent<Actions>()) {
                    pc++;

                    auto action = static_cast<Actions::Action>(*pc);
                    act->addAction(action);
                } else {
                    LOGW("SCRIPT ACTION: Missing Actions component");
                }

                break;
            case scripting::Instruction::MOVE:
            {
                LOGV("MOVE");

                auto y_index = POP();
                auto x_index = POP();

                GET_TARGET;
                if (target) {
                    if (auto move = target->getComponent<Movement>()) {
                        // Extract args

                        auto vel_x = program.getFloat(x_index) * (move->isFacingRight() ? 1.0 : -1.0);
                        auto vel_y = program.getFloat(y_index);

                        move->setVelocity(move->getVelX() + vel_x, move->getVelY() + vel_y);
                    } else {
                        LOGW("SCRIPT MOVE: Missing Movement component");
                    }
                }

                break;
            }
            case scripting::Instruction::SET_VELOCITY:
            {
                LOGV("SET_VELOCITY");

                auto y_index = POP();
                auto x_index = POP();

                GET_TARGET;
                if (target) {
                    if (auto move = target->getComponent<Movement>()) {
                        // Extract args

                        auto vel_x = program.getFloat(x_index);
                        auto vel_y = program.getFloat(y_index);

                        move->setVelocity(vel_x, vel_y);
                    } else {
                        LOGW("SCRIPT SET_VELOCITY: Missing Movement component");
                    }
                }

                break;
            }
            case scripting::Instruction::SET_POSITION:
            {
                LOGV("SET_POSITION");

                auto pos_y = POP();
                auto pos_x = POP();

                GET_TARGET;
                if (target) {
                    if (auto trans = target->getComponent<Transform>()) {
                        trans->setPosition(pos_x, pos_y);
                    } else {
                        LOGW("SCRIPT SET_POSITION: Missing Transform component");
                    }
                }

                break;
            }
            case scripting::Instruction::ENABLE_ACTION:
                LOGV("ENABLE_ACTION");

                if (auto action = extra_input.this_components->getComponent<Actions>()) {
                    pc++;
                    auto act = *pc;

                    action->enableAction(static_cast<Actions::Action>(act), true);
                } else {
                    LOGW("SCRIPT ENABLE_ACTION: Missing Actions component");
                }

                break;
            case scripting::Instruction::DISABLE_ACTION:
                LOGV("DISABLE_ACTION");

                if (auto action = extra_input.this_components->getComponent<Actions>()) {
                    pc++;
                    auto act = *pc;

                    action->enableAction(static_cast<Actions::Action>(act), false);
                } else {
                    LOGW("SCRIPT DISABLE_ACTION: Missing Actions component");
                }

                break;
            case scripting::Instruction::ADD_SHADER_TO_LAYER:
                LOGV("ADD_SHADER_TO_LAYER");

                if (auto rndr = extra_input.this_components->getComponent<Rendering>()) {
                    int layer = POP();

                    // Decrase by one to change indexing
                    int shader_id = POP() - 1;

                    auto shaders = rndr->getShaders();

                    // If 0 or negative, add all
                    if (shader_id < 0) {
                        for (auto shader : shaders) {
                            System::getRender()->addShader(shader, layer);
                        }
                    } else {
                        auto shader_index = static_cast<unsigned long long>(shader_id);
                        if (shaders.size() > shader_index) {
                            System::getRender()->addShader(shaders.at(shader_index), layer);
                        } else {
                            LOGW("SCRIPT ADD_SHADER_TO_LAYER: Unknown shader id %i", shader_id);
                        }
                    }
                } else {
                    LOGW("SCRIPT ADD_SHADER_TO_LAYER: Missing Rendering component");
                }

                break;
            case scripting::Instruction::ADD_GLOBAL_SHADER:
                LOGV("ADD_GLOBAL_SHADER");

                if (auto rndr = extra_input.this_components->getComponent<Rendering>()) {
                    // Decrase by one to change indexing
                    int shader_id = POP() - 1;

                    auto shaders = rndr->getShaders();

                    // If 0 or negative, add all
                    if (shader_id < 0) {
                        for (auto shader : shaders) {
                            System::getRender()->addGlobalShader(shader);
                        }
                    } else {
                        auto shader_index = static_cast<unsigned long long>(shader_id);
                        if (shaders.size() > shader_index) {
                            System::getRender()->addGlobalShader(shaders.at(shader_index));
                        } else {
                            LOGW("SCRIPT ADD_GLOBAL_SHADER: Unknown shader id %i", shader_id);
                        }
                    }
                } else {
                    LOGW("SCRIPT ADD_GLOBAL_SHADER: Missing Rendering component");
                }

                break;
            case scripting::Instruction::ADD_CAMERA_TRAUMA:
            {
                LOGV("ADD_CAMERA_TRAUMA");

                auto trauma_index = POP();
                System::getCamera()->addTrauma(program.getFloat(trauma_index));

                break;
            }
            case scripting::Instruction::IF:
            {
                LOGV("IF");
                auto condition = POP();

                // If condition is not true ignore rest of program
                if (condition != Bool::TRUE) {
                    pc = byte_code.end();

                    // Empty stack and push 0 return value
                    while (!stack.empty()) {
                        POP();
                    }

                    PUSH(0);
                }

                // Else just continue executing body

                break;
            }
            case scripting::Instruction::THEN:
                // NOP
                break;
            default:
                LOGW("Invalid operation %i", *pc);
                return 0;
        }
    }

    if (!stack.empty()) {
        return POP();
    } else {
        return 0;
    }
}


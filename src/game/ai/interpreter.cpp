#include "interpreter.h"

#include <stack>

#include "system/system.h"

#include "utils/log.h"

using ai::Bool;
using ai::Target;

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
        LOGW("AI: Stack is empty");
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
        LOGV("AI COMMAND START");
        switch (static_cast<ai::Instruction>(*pc)) {
            case ai::Instruction::INT:
                LOGV("INT");
                pc++;

                LOGV("%i", *pc);

                PUSH(*pc);
                break;
            case ai::Instruction::BOOL:
                LOGV("BOOL");
                pc++;
                PUSH(*pc);
                break;
            case ai::Instruction::STRING:
                LOGV("STRING");
                pc++;

                LOGV("%i", *pc);

                PUSH(*pc);
                break;
            case ai::Instruction::FLOAT:
                LOGV("FLOAT");
                pc++;

                LOGV("%i", *pc);

                PUSH(*pc);
                break;
            case ai::Instruction::PLAYER:
                LOGV("PLAYER");
                PUSH(Target::PLAYER);
                break;
            case ai::Instruction::THIS:
                LOGV("THIS");
                PUSH(Target::THIS);
                break;
            case ai::Instruction::POSITION_X:
            {
                LOGV("POSITION_X");

                GET_TARGET;
                if (target) {
                    if (auto trans = target->getComponent<Transform>()) {
                        PUSH(trans->getX());
                        break;
                    } else {
                        LOGW("AI POSITION_X: Missing Transform component");
                    }
                }

                PUSH(0);
                break;
            }
            case ai::Instruction::FRAME_TIMER:
                LOGV("FRAME_TIMER");
                if (static_cast<int>(extra_input.frame_timer) >= POP()) {
                    PUSH(Bool::TRUE);
                } else {
                    PUSH(Bool::FALSE);
                }
                break;
            case ai::Instruction::COLLIDES:
                LOGV("COLLIDES");
                if (POP() == ai::Target::PLAYER) {
                    if (auto this_coll = extra_input.this_components->getComponent<Collision>()) {
                        if (this_coll->collides(player_components->getComponent<Collision>())) {
                            PUSH(Bool::TRUE);
                        } else {
                            PUSH(Bool::FALSE);
                        }
                    } else {
                        LOGW("AI COLLIDES: Missing Collision component");
                        PUSH(Bool::FALSE);
                    }
                } else {
                    PUSH(Bool::FALSE);
                }
                break;
            case ai::Instruction::FLAG:
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
            case ai::Instruction::ANIMATION_LOOPED:
                LOGV("ANIMATION_LOOPED");
                if (auto anim = extra_input.this_components->getComponent<Animation>()) {
                    if (anim->hasAnimationLooped()) {
                        PUSH(Bool::TRUE);
                    } else {
                        PUSH(Bool::FALSE);
                    }
                } else {
                    LOGW("AI ANIMATION_LOOPED: Missing Animation component");
                    PUSH(Bool::FALSE);
                }
                break;
            case ai::Instruction::SENSOR:
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
                    LOGW("AI SENSOR: Missing Collision component");
                    PUSH(Bool::FALSE);
                }
                break;
            }
            case ai::Instruction::GRT:
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
            case ai::Instruction::LSS:
            {
                LOGV("LSS");
                auto a = POP();
                auto b = POP();

                LOGV("%i", a);
                LOGV("%i", b);

                PUSH(b < a ? Bool::TRUE : Bool::FALSE);
                break;
            }
            case ai::Instruction::AND:
            {
                LOGV("AND");
                auto a = POP();
                auto b = POP();

                LOGV("%i", a);
                LOGV("%i", b);

                PUSH(a && b ? Bool::TRUE : Bool::FALSE);
                break;
            }
            case ai::Instruction::OR:
            {
                LOGV("OR");
                auto a = POP();
                auto b = POP();

                LOGV("%i", a);
                LOGV("%i", b);

                PUSH(a || b ? Bool::TRUE : Bool::FALSE);
                break;
            }
            case ai::Instruction::ACTION:
                LOGV("ACTION");

                if (auto act = extra_input.this_components->getComponent<Actions>()) {
                    pc++;

                    auto action = static_cast<Actions::Action>(*pc);
                    act->addAction(action);
                } else {
                    LOGW("AI ACTION: Missing Actions component");
                }

                break;
            case ai::Instruction::MOVE:
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
                        LOGW("AI MOVE: Missing Movement component");
                    }
                }

                break;
            }
            case ai::Instruction::SET_VELOCITY:
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
                        LOGW("AI SET_VELOCITY: Missing Movement component");
                    }
                }

                break;
            }
            case ai::Instruction::ENABLE_ACTION:
                LOGV("ENABLE_ACTION");

                if (auto action = extra_input.this_components->getComponent<Actions>()) {
                    pc++;
                    auto act = *pc;

                    action->enableAction(static_cast<Actions::Action>(act), true);
                } else {
                    LOGW("AI ENABLE_ACTION: Missing Actions component");
                }

                break;
            case ai::Instruction::DISABLE_ACTION:
                LOGV("DISABLE_ACTION");

                if (auto action = extra_input.this_components->getComponent<Actions>()) {
                    pc++;
                    auto act = *pc;

                    action->enableAction(static_cast<Actions::Action>(act), false);
                } else {
                    LOGW("AI DISABLE_ACTION: Missing Actions component");
                }

                break;
            case ai::Instruction::RESET_JUMPS:
                LOGV("RESET_JUMPS");
                if (auto physics = extra_input.this_components->getComponent<Physics>()) {
                    physics->resetJumps(POP());
                }
                break;
            case ai::Instruction::RESET_DASHES:
                LOGV("RESET_DASHES");
                if (auto physics = extra_input.this_components->getComponent<Physics>()) {
                    physics->resetDashes(POP());
                }
                break;
            case ai::Instruction::ADD_SHADER_TO_LAYER:
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
                            LOGW("AI ADD_SHADER_TO_LAYER: Unknown shader id %i", shader_id);
                        }
                    }
                } else {
                    LOGW("AI ADD_SHADER_TO_LAYER: Missing Rendering component");
                }

                break;
            case ai::Instruction::ADD_GLOBAL_SHADER:
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
                            LOGW("AI ADD_GLOBAL_SHADER: Unknown shader id %i", shader_id);
                        }
                    }
                } else {
                    LOGW("AI ADD_GLOBAL_SHADER: Missing Rendering component");
                }

                break;
            case ai::Instruction::IF:
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
            case ai::Instruction::THEN:
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


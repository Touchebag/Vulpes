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

const Bimap<Actions::Action, state_utils::Event> action_event_map = {
    #define GENERATE_ENUM(action, name) {Actions::Action::action, state_utils::Event::ACTION_##action},
    #include "components/actions/actions_enum.h"
    #undef GENERATE_ENUM
};

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
                    }
                }

                PUSH(0);
                break;
            }
            case ai::Instruction::FRAME_TIMER:
                LOGV("FRAME_TIMER");
                if (extra_input.frame_timer >= POP()) {
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
                }
                break;
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

                    if (auto state = extra_input.this_components->getComponent<Stateful>()) {
                        if (action != Actions::Action::DIE) {
                            state->incomingEvent(action_event_map.at(action));
                        }
                    }
                } else {
                    LOGW("AI ACTION: Missing Actions component");
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
                        if (shaders.size() > static_cast<unsigned long long>(shader_id)) {
                            System::getRender()->addShader(shaders.at(shader_id), layer);
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
                        if (shaders.size() > static_cast<unsigned long long>(shader_id)) {
                            System::getRender()->addGlobalShader(shaders.at(shader_id));
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


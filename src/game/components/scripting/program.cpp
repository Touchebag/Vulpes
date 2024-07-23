#include "program.h"

#include "components/collision/collideables/collideable_sensor.h"

#include "utils/log.h"

#include <sstream>
#include <algorithm>
#include <regex>

#include "lexer.h"
#include "parser.h"

#include "components/component_store.h"
#include "system/system.h"

#define GET_ARG(x) args_retvals.at(x)

#define GET_TARGET(x) \
    std::shared_ptr<ComponentStore> target; \
    if (x == static_cast<int>(scripting::Target::PLAYER)) { \
        if (auto player = System::IWorldModify::getPlayer().lock()) { \
            target = player->components_; \
        } else { \
            LOGE("Scripting: Failed to get player"); \
            exit(EXIT_FAILURE);\
        } \
    } else if (x == static_cast<int>(scripting::Target::THIS)) { \
        target = extra_data.this_components; \
    } else { \
        LOGE("Scripting: Invalid target"); \
        exit(EXIT_FAILURE);\
    }

Program Program::loadProgram(const std::string& program_string) {
    Program program_out;

    Lexer lexer{};
    auto lexed_program = lexer.tokenizeProgram(program_string);

    Parser parser{};
    auto ast = parser.generateAST(lexed_program);

    if (ast.on_enter) {
        program_out.meta_data_ = MetaData::ON_ENTER;
    } else if (ast.on_exit) {
        program_out.meta_data_ = MetaData::ON_EXIT;
    }

    program_out.ast_ = ast;

    return program_out;
}

scripting::return_types Program::run(Program::ExtraInputData extra_data) const {
    return executeInstruction(ast_.operation, extra_data);
}

scripting::return_types Program::executeInstruction(scripting::Operation oper, Program::ExtraInputData& extra_data) const {
    scripting::return_types result = 12345;

    std::vector<scripting::return_types> args_retvals = {};
    if (oper.instruction == scripting::Instruction::IF) {
        // Only evaluate first argument to drop early if false
        args_retvals.push_back(executeInstruction(oper.arguments.at(0), extra_data));
    } else {
        for (auto i : oper.arguments) {
            // Evaluate arguments
            args_retvals.push_back(executeInstruction(i, extra_data));
        }
    }

    switch (oper.instruction) {
        case (scripting::Instruction::INT):
            result = oper.data;
            break;
        case (scripting::Instruction::BOOL):
            result = oper.data;
            break;
        case (scripting::Instruction::FLOAT):
            result = oper.data;
            break;
        case (scripting::Instruction::STRING):
            result = oper.data;
            break;
        case (scripting::Instruction::PLAYER):
            result = static_cast<int>(scripting::Target::PLAYER);
            break;
        case (scripting::Instruction::THIS):
            result = static_cast<int>(scripting::Target::THIS);
            break;
        case (scripting::Instruction::TOP_EDGE):
            result = static_cast<int>(scripting::CollideableProperty::TOP_EDGE);
            break;
        case (scripting::Instruction::BOTTOM_EDGE):
            result = static_cast<int>(scripting::CollideableProperty::BOTTOM_EDGE);
            break;
        case (scripting::Instruction::LEFT_EDGE):
            result = static_cast<int>(scripting::CollideableProperty::LEFT_EDGE);
            break;
        case (scripting::Instruction::RIGHT_EDGE):
            result = static_cast<int>(scripting::CollideableProperty::RIGHT_EDGE);
            break;
        case (scripting::Instruction::ACTION_LITERAL):
            result = oper.data;
            break;
        case (scripting::Instruction::SET):
        {
            auto var_name = std::get<std::string>(GET_ARG(0));
            extra_data.variables->insert_or_assign(var_name, std::get<int>(GET_ARG(1)));

            result = 0;
            break;
        }
        case scripting::Instruction::GET:
        {
            auto var_name = std::get<std::string>(GET_ARG(0));
            auto value = extra_data.variables->at(var_name);

            result = value;
            break;
        }
        case (scripting::Instruction::INC_VAR):
        {
            auto var_name = std::get<std::string>(GET_ARG(0));
            auto value = extra_data.variables->at(var_name);
            extra_data.variables->insert_or_assign(var_name, ++value);

            result = value;
            break;
        }
        case (scripting::Instruction::DEC_VAR):
        {
            auto var_name = std::get<std::string>(GET_ARG(0));
            auto value = extra_data.variables->at(var_name);
            extra_data.variables->insert_or_assign(var_name, --value);

            result = value;
            break;
        }
        case (scripting::Instruction::FRAME_TIMER):
        {
            auto expected_frame = std::get<int>(GET_ARG(0));

            result = (expected_frame <= static_cast<int>(extra_data.frame_timer));
            break;
        }
        case (scripting::Instruction::ADD):
            result = (std::get<int>(GET_ARG(0)) + std::get<int>(GET_ARG(1)));
            break;
        case (scripting::Instruction::SUB):
            result = (std::get<int>(GET_ARG(0)) - std::get<int>(GET_ARG(1)));
            break;
        case (scripting::Instruction::POSITION_X):
        {
            GET_TARGET(std::get<int>(GET_ARG(0)));
            if (target) {
                if (auto trans = target->getComponent<Transform>()) {
                    result = trans->getX();
                    break;
                } else {
                    LOGW("SCRIPT POSITION_X: Missing Transform component");
                }
            }
            break;
        }
        case (scripting::Instruction::POSITION_Y):
        {
            GET_TARGET(std::get<int>(GET_ARG(0)));
            if (target) {
                if (auto trans = target->getComponent<Transform>()) {
                    result = trans->getY();
                    break;
                } else {
                    LOGW("SCRIPT POSITION_Y: Missing Transform component");
                }
            }
            break;
        }
        case scripting::Instruction::SENSOR:
        {
            auto sensor = std::get<std::string>(GET_ARG(0));

            if (auto coll = extra_data.this_components->getComponent<Collision>()) {
                result = coll->isSensorTriggered(sensor);
            } else {
                LOGW("SCRIPT SENSOR: Missing Collision component");
                result = false;
            }
            break;
        }
        case scripting::Instruction::PENETRATION_DISTANCE:
        {
            auto sensor_name = std::get<std::string>(GET_ARG(0));
            auto target_prop = std::get<int>(GET_ARG(1));

            if (auto coll = extra_data.this_components->getComponent<Collision>()) {
                if (std::shared_ptr<CollideableSensor> sens = coll->getSensor(sensor_name)) {
                    result = sens->getPenetrationDistance(static_cast<scripting::CollideableProperty>(target_prop));
                } else {
                    throw std::invalid_argument(std::string("SCRIPT PENETRATION_DISTANCE: Sensor \"") + sensor_name + "\" not found");
                }
            }

            break;
        }
        case scripting::Instruction::COLLIDES:
            if (std::get<int>(GET_ARG(0)) == static_cast<int>(scripting::Target::PLAYER)) {
                if (auto this_coll = extra_data.this_components->getComponent<Collision>()) {
                    if (auto player = System::IWorldModify::getPlayer().lock()) {
                        if (this_coll->collides(player->components_->getComponent<Collision>())) {
                            result = true;
                        } else {
                            result = false;
                        }
                    } else {
                        result = false;
                    }
                } else {
                    LOGW("SCRIPT COLLIDES: Missing Collision component");
                    result = false;
                }
            } else {
                result = false;
            }
            break;
        case scripting::Instruction::FLAG:
            result = System::getEnvironment()->getFlag(std::get<std::string>(GET_ARG(0)));
            break;
        case scripting::Instruction::ANIMATION_LOOPED:
            if (auto anim = extra_data.this_components->getComponent<Animation>()) {
                result = anim->hasAnimationLooped();
            } else {
                LOGW("SCRIPT ANIMATION_LOOPED: Missing Animation component");
                result = false;
            }
            break;
        case scripting::Instruction::GRT:
            result = std::get<int>(GET_ARG(0)) > std::get<int>(GET_ARG(1));
            break;
        case scripting::Instruction::LSS:
            result = std::get<int>(GET_ARG(0)) < std::get<int>(GET_ARG(1));
            break;
        case scripting::Instruction::NOT:
            result = !std::get<bool>(GET_ARG(0));
            break;
        case scripting::Instruction::AND:
            result = std::get<bool>(GET_ARG(0)) && std::get<bool>(GET_ARG(1));
            break;
        case scripting::Instruction::OR:
            result = std::get<bool>(GET_ARG(0)) || std::get<bool>(GET_ARG(1));
            break;
        case scripting::Instruction::IF:
            // If condition is true, evalute second argument
            if (std::get<bool>(GET_ARG(0))) {
                // Skip "then"
                result = executeInstruction(oper.arguments.at(2), extra_data);
            } else {
                result = 0;
            }

            break;
        case scripting::Instruction::THEN:
            // NOP
            break;
        case scripting::Instruction::ACTION:
            if (auto act = extra_data.this_components->getComponent<Actions>()) {
                auto action = static_cast<Actions::Action>(std::get<int>(GET_ARG(0)));
                act->addAction(action);
            } else {
                LOGW("SCRIPT ACTION: Missing Actions component");
            }

            result = 0;
            break;
        case scripting::Instruction::ADD_CAMERA_TRAUMA:
            System::getCamera()->addTrauma(std::get<double>(GET_ARG(0)));
            result = 0;
            break;
        case scripting::Instruction::MOVE:
        {
            auto vel_x = std::get<double>(GET_ARG(1));
            auto vel_y = std::get<double>(GET_ARG(2));
            GET_TARGET(std::get<int>(GET_ARG(0)));

            if (target) {
                if (auto move = target->getComponent<Movement>()) {
                    // Adjust to facing direction
                    vel_x = vel_x * (move->isFacingRight() ? 1.0 : -1.0);
                    move->setVelocity(move->getVelX() + vel_x, move->getVelY() + vel_y);
                } else {
                    LOGW("SCRIPT MOVE: Missing Movement component");
                }
            }

            result = 0;
            break;
        }
        case scripting::Instruction::SET_VELOCITY:
        {
            auto vel_x = std::get<double>(GET_ARG(1));
            auto vel_y = std::get<double>(GET_ARG(2));
            GET_TARGET(std::get<int>(GET_ARG(0)));

            if (target) {
                if (auto move = target->getComponent<Movement>()) {
                    move->setVelocity(vel_x, vel_y);
                } else {
                    LOGW("SCRIPT SET_VELOCITY: Missing Movement component");
                }
            }

            result = 0;
            break;
        }
        case scripting::Instruction::SET_POSITION:
        {
            auto pos_x = std::get<int>(GET_ARG(1));
            auto pos_y = std::get<int>(GET_ARG(2));
            GET_TARGET(std::get<int>(GET_ARG(0)));

            if (target) {
                if (auto trans = target->getComponent<Transform>()) {
                    trans->setPosition(pos_x, pos_y);
                } else {
                    LOGW("SCRIPT SET_POSITION: Missing Transform component");
                }
            }

            result = 0;
            break;
        }
        case scripting::Instruction::ENABLE_ACTION:
            if (auto action = extra_data.this_components->getComponent<Actions>()) {
                action->enableAction(static_cast<Actions::Action>(std::get<int>(GET_ARG(0))), true);
            } else {
                LOGW("SCRIPT ENABLE_ACTION: Missing Actions component");
            }

            result = 0;
            break;
        case scripting::Instruction::DISABLE_ACTION:
            if (auto action = extra_data.this_components->getComponent<Actions>()) {
                action->enableAction(static_cast<Actions::Action>(std::get<int>(GET_ARG(0))), false);
            } else {
                LOGW("SCRIPT DISABLE_ACTION: Missing Actions component");
            }

            result = 0;
            break;
        case scripting::Instruction::ADD_SHADER_TO_LAYER:
            if (auto rndr = extra_data.this_components->getComponent<Rendering>()) {
                int shader_id = std::get<int>(GET_ARG(0)) - 1;
                int layer = std::get<int>(GET_ARG(1));

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

            result = 0;
            break;
        case scripting::Instruction::ADD_GLOBAL_SHADER:
            if (auto rndr = extra_data.this_components->getComponent<Rendering>()) {
                int shader_id = std::get<int>(GET_ARG(0)) - 1;

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

            result = 0;
            break;
        case scripting::Instruction::INVALID:
            throw std::invalid_argument("Invalid instruction");
            break;
    }

    return result;
}

const std::string& Program::getString(int id) {
    return strings_.at(id);
}

double Program::getFloat(int id) {
    return floats_.at(id);
}

Program::MetaData Program::getMetaData() {
    return meta_data_;
}

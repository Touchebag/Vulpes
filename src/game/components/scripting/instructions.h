#pragma once

#include <unordered_map>

namespace scripting {

enum class Instruction {
    // Literal values
    INT,
    BOOL,
    STRING,
    FLOAT,

    // Dynamic values
    FRAME_TIMER,

    // Targets
    PLAYER,
    THIS,

    // Properties
    POSITION_X,

    // Checks
    COLLIDES,
    FLAG,
    ANIMATION_LOOPED,
    SENSOR,

    // Comparisons
    GRT,
    LSS,

    // Logical operators
    AND,
    OR,

    // If statement
    IF,
    THEN,

    // Actions
    ACTION,

    // Component modifications
    MOVE,
    SET_VELOCITY,

    ENABLE_ACTION,
    DISABLE_ACTION,

    RESET_JUMPS,
    RESET_DASHES,

    ADD_SHADER_TO_LAYER,
    ADD_GLOBAL_SHADER,

    ADD_CAMERA_TRAUMA,
};

enum class Type {
    // Used for editor
    NONE,

    VOID,

    INT,
    BOOL,
    FLOAT,

    STRING,

    // Typed target
    TARGET,

    // To help with parsing
    THEN,
};

enum Bool {
    FALSE = 0,
    TRUE = 1,
};

enum Target {
    THIS,
    PLAYER,
};

struct InstructionData {
    Instruction instruction;
    Type return_type;
    std::vector<Type> args_return_type;
};

static const std::unordered_map<std::string, InstructionData> string_instruction_map = {
    {"frame_timer", {Instruction::FRAME_TIMER, Type::BOOL, {Type::INT}}},

    {"player", {Instruction::PLAYER, Type::TARGET, {}}},
    {"this", {Instruction::THIS, Type::TARGET, {}}},

    {"position_x", {Instruction::POSITION_X, Type::INT, {Type::TARGET}}},

    {"collides", {Instruction::COLLIDES, Type::BOOL, {Type::TARGET}}},
    {"flag", {Instruction::FLAG, Type::BOOL, {Type::STRING}}},
    {"animation_looped", {Instruction::ANIMATION_LOOPED, Type::BOOL, {}}},
    {"sensor", {Instruction::SENSOR, Type::BOOL, {Type::STRING}}},

    {"grt", {Instruction::GRT, Type::BOOL, {Type::INT, Type::INT}}},
    {"lss", {Instruction::LSS, Type::BOOL, {Type::INT, Type::INT}}},

    {"and", {Instruction::AND, Type::BOOL, {Type::BOOL, Type::BOOL}}},
    {"or", {Instruction::OR, Type::BOOL, {Type::BOOL, Type::BOOL}}},

    {"if", {Instruction::IF, Type::VOID, {Type::BOOL, Type::THEN, Type::VOID}}},
    {"then", {Instruction::THEN, Type::THEN, {}}},

    {"action", {Instruction::ACTION, Type::VOID, {Type::STRING}}},

    {"move", {Instruction::MOVE, Type::VOID, {Type::TARGET, Type::FLOAT, Type::FLOAT}}},
    {"set_velocity", {Instruction::SET_VELOCITY, Type::VOID, {Type::TARGET, Type::FLOAT, Type::FLOAT}}},

    {"enable_action", {Instruction::ENABLE_ACTION, Type::VOID, {Type::STRING}}},
    {"disable_action", {Instruction::DISABLE_ACTION, Type::VOID, {Type::STRING}}},

    {"reset_jumps", {Instruction::RESET_JUMPS, Type::VOID, {Type::INT}}},
    {"reset_dashes", {Instruction::RESET_DASHES, Type::VOID, {Type::INT}}},

    {"add_shader_to_layer", {Instruction::ADD_SHADER_TO_LAYER, Type::VOID, {Type::INT, Type::INT}}},
    {"add_global_shader", {Instruction::ADD_GLOBAL_SHADER, Type::VOID, {Type::INT}}},

    {"add_camera_trauma", {Instruction::ADD_CAMERA_TRAUMA, Type::VOID, {Type::FLOAT}}},
};

} // scripting

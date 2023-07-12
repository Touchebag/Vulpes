#pragma once

#include <unordered_map>

namespace scripting {

enum class Instruction {
    // Literal values
    INT,
    BOOL,
    STRING,
    FLOAT,

    // Variables
    GET,
    SET,

    INC_VAR, // Convenience
    DEC_VAR, // Convenience

    // Dynamic values
    FRAME_TIMER,

    // Arithmetic operations
    ADD,
    SUB,

    // Targets
    PLAYER,
    THIS,

    // Properties
    POSITION_X,
    POSITION_Y,

    // Collision history
    COLLISION_HISTORY,

    // Collision properties
    TOP_EDGE,
    BOTTOM_EDGE,
    LEFT_EDGE,
    RIGHT_EDGE,

    // Checks
    COLLIDES,
    FLAG,
    ANIMATION_LOOPED,
    SENSOR,

    // Comparisons
    GRT,
    LSS,

    // Logical operators
    NOT,
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
    SET_POSITION,

    ENABLE_ACTION,
    DISABLE_ACTION,

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

    COLL_PROP,

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

enum CollideableProperty {
    TOP_EDGE,
    BOTTOM_EDGE,
    LEFT_EDGE,
    RIGHT_EDGE,
};

struct InstructionData {
    Instruction instruction;
    Type return_type;
    std::vector<Type> args_return_type;
};

static const std::unordered_map<std::string, InstructionData> string_instruction_map = {
    {"get", {Instruction::GET, Type::INT, {Type::STRING}}},
    {"set", {Instruction::SET, Type::VOID, {Type::STRING, Type::INT}}},

    {"inc_var", {Instruction::INC_VAR, Type::VOID, {Type::STRING}}},
    {"dec_var", {Instruction::DEC_VAR, Type::VOID, {Type::STRING}}},

    {"frame_timer", {Instruction::FRAME_TIMER, Type::BOOL, {Type::INT}}},

    {"player", {Instruction::PLAYER, Type::TARGET, {}}},
    {"this", {Instruction::THIS, Type::TARGET, {}}},

    {"top_edge", {Instruction::TOP_EDGE, Type::COLL_PROP, {}}},
    {"bottom_edge", {Instruction::BOTTOM_EDGE, Type::COLL_PROP, {}}},
    {"left_edge", {Instruction::LEFT_EDGE, Type::COLL_PROP, {}}},
    {"right_edge", {Instruction::RIGHT_EDGE, Type::COLL_PROP, {}}},

    {"add", {Instruction::ADD, Type::INT, {Type::INT, Type::INT}}},
    {"sub", {Instruction::SUB, Type::INT, {Type::INT, Type::INT}}},

    {"position_x", {Instruction::POSITION_X, Type::INT, {Type::TARGET}}},
    {"position_y", {Instruction::POSITION_Y, Type::INT, {Type::TARGET}}},

    {"collision_history", {Instruction::COLLISION_HISTORY, Type::INT, {Type::STRING, Type::INT, Type::COLL_PROP}}},

    {"collides", {Instruction::COLLIDES, Type::BOOL, {Type::TARGET}}},
    {"flag", {Instruction::FLAG, Type::BOOL, {Type::STRING}}},
    {"animation_looped", {Instruction::ANIMATION_LOOPED, Type::BOOL, {}}},
    {"sensor", {Instruction::SENSOR, Type::BOOL, {Type::STRING}}},

    {"grt", {Instruction::GRT, Type::BOOL, {Type::INT, Type::INT}}},
    {"lss", {Instruction::LSS, Type::BOOL, {Type::INT, Type::INT}}},

    {"not", {Instruction::NOT, Type::BOOL, {Type::BOOL}}},
    {"and", {Instruction::AND, Type::BOOL, {Type::BOOL, Type::BOOL}}},
    {"or", {Instruction::OR, Type::BOOL, {Type::BOOL, Type::BOOL}}},

    {"if", {Instruction::IF, Type::VOID, {Type::BOOL, Type::THEN, Type::VOID}}},
    {"then", {Instruction::THEN, Type::THEN, {}}},

    {"action", {Instruction::ACTION, Type::VOID, {Type::STRING}}},

    {"move", {Instruction::MOVE, Type::VOID, {Type::TARGET, Type::FLOAT, Type::FLOAT}}},
    {"set_velocity", {Instruction::SET_VELOCITY, Type::VOID, {Type::TARGET, Type::FLOAT, Type::FLOAT}}},
    {"set_position", {Instruction::SET_POSITION, Type::VOID, {Type::TARGET, Type::INT, Type::INT}}},

    {"enable_action", {Instruction::ENABLE_ACTION, Type::VOID, {Type::STRING}}},
    {"disable_action", {Instruction::DISABLE_ACTION, Type::VOID, {Type::STRING}}},

    {"add_shader_to_layer", {Instruction::ADD_SHADER_TO_LAYER, Type::VOID, {Type::INT, Type::INT}}},
    {"add_global_shader", {Instruction::ADD_GLOBAL_SHADER, Type::VOID, {Type::INT}}},

    {"add_camera_trauma", {Instruction::ADD_CAMERA_TRAUMA, Type::VOID, {Type::FLOAT}}},
};

using VariableMap = std::unordered_map<std::string, int>;

} // scripting

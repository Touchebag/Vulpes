#pragma once

#include <unordered_map>

namespace ai {

enum class Instruction {
    // Literal values
    INT,
    BOOL,
    STRING,

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

    // Comparisons
    GRT,
    LSS,

    // Logical operators
    AND,
    OR,

    // If statement
    IF,
    THEN,

    // This is used to restrict access as needed
    READ_ONLY,

    // Actions
    ACTION,

    ADD_SHADER_TO_LAYER,
};

enum class Type {
    VOID,

    INT,
    BOOL,

    // To avoid mixups with normal ints
    TARGET,

    STRING,

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

    {"grt", {Instruction::GRT, Type::BOOL, {Type::INT, Type::INT}}},
    {"lss", {Instruction::LSS, Type::BOOL, {Type::INT, Type::INT}}},

    {"and", {Instruction::AND, Type::BOOL, {Type::BOOL, Type::BOOL}}},
    {"or", {Instruction::OR, Type::BOOL, {Type::BOOL, Type::BOOL}}},

    {"if", {Instruction::IF, Type::VOID, {Type::BOOL, Type::THEN, Type::VOID}}},
    {"then", {Instruction::THEN, Type::THEN, {}}},

    {"action", {Instruction::ACTION, Type::VOID, {Type::STRING}}},

    {"add_shader_to_layer", {Instruction::ADD_SHADER_TO_LAYER, Type::VOID, {Type::INT, Type::INT}}},
};

} // ai

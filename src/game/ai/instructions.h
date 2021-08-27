#pragma once

#include <unordered_map>

namespace ai {

enum class Instruction {
    // Literal values
    INT,
    TRUE,
    FALSE,

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

    // Comparisons
    GRT,
    LSS,

    // Logical operators
    AND,
    OR,

    // This is used to restrict access as needed
    READ_ONLY,

    // Actions
    ACTION,
};

struct InstructionData {
    Instruction instruction;
    int number_of_args;
};

std::unordered_map<std::string, InstructionData> string_instruction_map = {
    {"int", {Instruction::INT, 1}},
    {"true", {Instruction::TRUE, 0}},
    {"false", {Instruction::FALSE, 0}},

    {"frame_timer", {Instruction::FRAME_TIMER, 1}},

    {"player", {Instruction::PLAYER, 1}},
    {"this", {Instruction::THIS, 1}},

    {"position_x", {Instruction::POSITION_X, 0}},

    {"collides", {Instruction::COLLIDES, 1}},
    {"flag", {Instruction::FLAG, 1}},

    {"grt", {Instruction::GRT, 2}},
    {"lss", {Instruction::LSS, 2}},

    {"and", {Instruction::AND, 2}},
    {"or", {Instruction::OR, 2}},

    {"action", {Instruction::ACTION, 1}},
};

} // ai

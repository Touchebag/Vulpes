#include "program.h"

#include "components/actions/common.h" // For string_action_map

#include "utils/log.h"

#include <sstream>
#include <algorithm>
#include <regex>

namespace {

void checkType(ai::Type expected, ai::Type actual) {
    // Void can return whatever
    if (expected != ai::Type::VOID && expected != actual) {
        std::stringstream error_message;
        error_message << "type error. Expected " << static_cast<int>(expected) <<
                         ", got " << static_cast<int>(actual);
        throw std::invalid_argument(error_message.str());
    }
}

std::vector<std::string> tokenizeString(std::string str) {
    std::vector<std::string> ret_vec;
    const std::regex re("[A-z0-9\\._'\\-]+|\\(|\\)");

    std::smatch sm;
    while (std::regex_search(str, sm, re)) {
        ret_vec.push_back(sm.str());
        str = sm.suffix();
    }

    return ret_vec;
}

ai::InstructionData parseInstruction(std::string instruction) {
    ai::InstructionData instruction_data;

    // Should be able to parse literal int without keyword
    try {
        std::unique_ptr<size_t> n = std::make_unique<size_t>();

        // Try read int
        (void)std::stoi(instruction, n.get());
        // Check if entire string could be parsed as int
        if (*n == instruction.length()) {
            instruction_data = {ai::Instruction::INT, ai::Type::INT, {}};
            return instruction_data;
        }

        // Try read double
        (void)std::stod(instruction, n.get());
        if (*n == instruction.length()) {
            instruction_data = {ai::Instruction::FLOAT, ai::Type::FLOAT, {}};
            return instruction_data;
        }
    } catch (std::invalid_argument&) {
        // If not int, just continue parsing as normal
    }

    if (instruction.front() == '\'') {
        if (instruction.back() != '\'') {
            throw std::invalid_argument("Program: Parse error, unmatched '");
        }

        instruction_data = {ai::Instruction::STRING, ai::Type::STRING, {}};

        return instruction_data;
    }

    if (instruction == "true" || instruction == "false") {
        instruction_data = {ai::Instruction::BOOL, ai::Type::BOOL, {}};

        return instruction_data;
    }

    try {
        instruction_data = ai::string_instruction_map.at(instruction);
    } catch (std::out_of_range& e) {
        throw std::invalid_argument("Program: Unknown instruction " + instruction);
    }

    return instruction_data;
}

// Convenience function to allow easier error handling of argument strings
ai::InstructionData parseInstruction(std::vector<std::string> instructions) {
    if (instructions.empty()) {
        throw std::invalid_argument("Program: Instruction list is empty");
    }

    return parseInstruction(instructions[0]);
}

std::vector<std::vector<std::string>> extractArguments(std::vector<std::string> str) {
    if (str.empty()) {
        return {};
    }

    std::vector<std::vector<std::string>> ret_vec;
    // Skip initial instruction
    auto start_it = str.begin() + 1;
    int paren_count = 0;

    for (auto it = start_it; it != str.end(); it++) {
        if (*it == "(") {
            if (paren_count == 0) {
                start_it = it;
            }
            paren_count++;
        } else if (*it == ")") {
            paren_count--;
            if (paren_count == 0) {
                ret_vec.push_back({start_it + 1, it});
            }
        } else {
            if (paren_count == 0) {
                ret_vec.push_back({*it});
            }
        }
    }

    if (paren_count != 0) {
        throw std::invalid_argument("Program: condition parse error, unmatched parentheses");
    }

    return ret_vec;
}

int parseAction(const std::string& action_string) {
    try {
        return static_cast<int>(string_action_map.at(action_string));
    } catch (std::out_of_range& e) {
        LOGE("Program, invalid action %s", action_string.c_str());
        throw;
    }
}

} // namespace

Program Program::loadProgram(const std::string& program_string) {
    Program program_out;

    auto lexed_program = tokenizeString(program_string);

    if (lexed_program[0] == "on_enter") {
        program_out.meta_data_ = MetaData::ON_ENTER;
        lexed_program.erase(lexed_program.begin());
    } else if (lexed_program[0] == "on_exit") {
        program_out.meta_data_ = MetaData::ON_EXIT;
        lexed_program.erase(lexed_program.begin());
    }

    program_out.translateAndStore(lexed_program);

    return program_out;
}

ai::Type Program::translateAndStore(std::vector<std::string> lexed_input) {
    // TODO Check length
    auto instruction_data = parseInstruction(lexed_input[0]);
    auto arguments = extractArguments(lexed_input);

    // Check correct number of arguments
    if (arguments.size() != instruction_data.args_return_type.size()) {
        std::stringstream error_message;
        error_message << "Instruction: " << lexed_input[0] <<
                         " incorrect number of arguments. Expected " << instruction_data.args_return_type.size() <<
                         ", got " << arguments.size();
        throw std::invalid_argument(error_message.str());
    }

    // Need to evaluate arguments first
    switch (instruction_data.instruction) {
        case ai::Instruction::INT:
            program_.push_back(static_cast<int>(ai::Instruction::INT));
            program_.push_back(std::stoi(lexed_input[0]));
            break;
        case ai::Instruction::BOOL:
            program_.push_back(static_cast<int>(ai::Instruction::BOOL));

            if (lexed_input[0] == "true") {
                program_.push_back(ai::Bool::TRUE);
            } else {
                program_.push_back(ai::Bool::FALSE);
            }

            break;
        case ai::Instruction::STRING:
            program_.push_back(static_cast<int>(ai::Instruction::STRING));

            // Remove surrounding quotation marks
            strings_.insert({string_id_counter_, std::string(lexed_input[0].begin() + 1, lexed_input[0].end() - 1)});
            program_.push_back(string_id_counter_);
            string_id_counter_++;
            break;
        case ai::Instruction::FLOAT:
            program_.push_back(static_cast<int>(ai::Instruction::FLOAT));

            floats_.insert({float_id_counter_, std::stod(lexed_input[0])});
            program_.push_back(float_id_counter_);
            float_id_counter_++;
            break;
        case ai::Instruction::ACTION:
            program_.push_back(static_cast<int>(ai::Instruction::ACTION));
            program_.push_back(parseAction(lexed_input[1]));
            break;
        case ai::Instruction::ENABLE_ACTION:
            program_.push_back(static_cast<int>(ai::Instruction::ENABLE_ACTION));
            program_.push_back(parseAction(lexed_input[1]));
            break;
        case ai::Instruction::DISABLE_ACTION:
            program_.push_back(static_cast<int>(ai::Instruction::DISABLE_ACTION));
            program_.push_back(parseAction(lexed_input[1]));
            break;
        case ai::Instruction::IF:
        {
            // Push condition
            auto condition = arguments[0];
            auto parsed_condition = parseInstruction(condition);

            checkType(instruction_data.args_return_type[0], parsed_condition.return_type);

            translateAndStore(condition);

            // Push IF statement
            program_.push_back(static_cast<int>(ai::Instruction::IF));

            // Ignore THEN

            // Push body
            auto body = arguments[2];
            auto parsed_body = parseInstruction(body);

            checkType(instruction_data.args_return_type[2], parsed_body.return_type);

            translateAndStore(body);

            break;
        }
        default:
            // Push arguments
            for (unsigned long long i = 0; i < arguments.size(); i++) {
                auto arg = arguments[i];
                auto parsed_arg = parseInstruction(arg);

                checkType(instruction_data.args_return_type[i], parsed_arg.return_type);

                translateAndStore(arg);
            }

            // Push original operation
            program_.push_back(static_cast<int>(instruction_data.instruction));

            break;
    }

    return instruction_data.return_type;
}

const std::vector<int> Program::getProgram() {
    return program_;
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

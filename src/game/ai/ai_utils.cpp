#include "ai_utils.h"

#include "utils/log.h"

#include <sstream>
#include <algorithm>
#include <regex>

namespace ai_utils {

std::vector<int> parseInstruction(ai::InstructionData data, std::vector<std::vector<std::string>> args) {
    std::vector<int> ret_program;

    auto instruction = data.instruction;

    // Need to evaluate arguments first
    switch (instruction) {
        case ai::Instruction::FLAG:
            LOGE("Fix strings");
            // TODO Do that ^
            ret_program.push_back(0);

            // Push original operation
            ret_program.push_back(static_cast<int>(instruction));
            break;
        case ai::Instruction::PLAYER:
        case ai::Instruction::THIS:
            // Push original operation
            ret_program.push_back(static_cast<int>(instruction));

            for (int i = 0; i < static_cast<int>(args.size()); i++) {
                auto parsed_arg = loadInstructions(args[i]);

                if (parsed_arg.second.return_type != data.args_return_type[i]) {
                    std::stringstream error_message;
                    error_message << "type error. Expected " << static_cast<int>(data.args_return_type[i]) <<
                                     ", got " << static_cast<int>(parsed_arg.second.return_type);
                    throw std::invalid_argument(error_message.str());
                }

                // Push arguments
                ret_program.insert(ret_program.end(), parsed_arg.first.begin(), parsed_arg.first.end());
            }
            break;
        default:
            for (int i = 0; i < static_cast<int>(args.size()); i++) {
                auto parsed_arg = loadInstructions(args[i]);

                if (parsed_arg.second.return_type != data.args_return_type[i]) {
                    std::stringstream error_message;
                    error_message << "type error. Expected " << static_cast<int>(data.args_return_type[i]) <<
                                     ", got " << static_cast<int>(parsed_arg.second.return_type);
                    throw std::invalid_argument(error_message.str());
                }

                // Push arguments
                ret_program.insert(ret_program.end(), parsed_arg.first.begin(), parsed_arg.first.end());
            }

            // Push original operation
            ret_program.push_back(static_cast<int>(instruction));

            break;
    }

    return ret_program;
}

std::vector<std::vector<std::string>> extractArguments(std::vector<std::string> str) {
    std::vector<std::vector<std::string>> ret_vec;
    auto start_it = str.begin();
    int paren_count = 0;

    if (str.empty()) {
        return {};
    }

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
        throw std::invalid_argument("AI condition parse error, unmatched parentheses");
    }

    return ret_vec;
}

std::pair<std::vector<int>, ai::InstructionData> loadInstructions(std::vector<std::string> lexed_input) {
    std::vector<int> instructions;

    std::vector<std::vector<std::string>> arguments = ai_utils::extractArguments(lexed_input);
    std::string operation = arguments[0][0];

    ai::InstructionData instruction_data;

    // Should be able to parse literal int without keyword
    try {
        auto int_literal = std::stoi(operation);

        instructions.push_back(static_cast<int>(ai::Instruction::INT));
        instructions.push_back(int_literal);

        instruction_data = ai::string_instruction_map.at("int");

        return {instructions, instruction_data};

    } catch (std::invalid_argument&) {
        // If not int, just continue parsing as normal
    }

    // Seperate catches to avoid multiple printouts due to recursion
    try {
        instruction_data = ai::string_instruction_map.at(operation);
    } catch (std::out_of_range& e) {
        LOGE("AI: Invalid operation %s", operation.c_str());
        throw e;
    }

    try {
        // Check correct number of arguments
        if (arguments.size() < instruction_data.args_return_type.size()) {
            std::stringstream error_message;
            error_message << "incorrect number of arguments. Expected " << instruction_data.args_return_type.size() <<
                             ", got " << arguments.size();
            throw std::invalid_argument(error_message.str());
        }

        instructions = parseInstruction(instruction_data, {arguments.begin() + 1, arguments.end()});
    } catch (std::invalid_argument& e) {
        std::stringstream error_message;
        error_message << "Operation " << operation << ": " << e.what();
        throw std::invalid_argument(error_message.str());
    }

    return {instructions, instruction_data};
}

std::vector<std::string> tokenizeString(std::string str) {
    std::vector<std::string> ret_vec;
    const std::regex re("[A-z0-9\\._]+|\\(|\\)");

    std::smatch sm;
    while (std::regex_search(str, sm, re)) {
        ret_vec.push_back(sm.str());
        str = sm.suffix();
    }

    return ret_vec;
}

} // ai_utils

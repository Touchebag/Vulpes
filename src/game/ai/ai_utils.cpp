#include "ai_utils.h"

#include "instructions.h"

#include "utils/log.h"

#include <sstream>
#include <algorithm>

namespace ai_utils {

std::vector<int> parseInstruction(ai::Instruction op, std::vector<std::string> args) {
    std::vector<int> instructions;

    instructions.push_back(static_cast<int>(op));

    for (auto arg : args) {
        auto parsed_arg = loadInstructions(arg);

        instructions.insert(instructions.end(), parsed_arg.begin(), parsed_arg.end());
    }

    return instructions;
}

std::vector<int> loadInstructions(std::string str) {
    std::vector<int> instructions;

    std::vector<std::string> arguments = ai_utils::extractArguments(str);
    std::string operation = arguments[0];

    try {
        auto int_literal = std::stoi(operation);

        instructions.push_back(static_cast<int>(ai::Instruction::INT));
        instructions.push_back(int_literal);
    } catch (std::invalid_argument&) {
        // If not int, just continue parsing as normal
    }

    try {
        auto instruction_data = ai::string_instruction_map.at(operation);

        // Check correct number of arguments
        if (static_cast<int>(arguments.size()) < instruction_data.number_of_args) {
            std::stringstream error_message;
            error_message << "Operation " << operation <<
                             " incorrect number of arguments. Expected " << instruction_data.number_of_args <<
                             ", got " << arguments.size();
            throw std::invalid_argument(error_message.str());
        }

        instructions = parseInstruction(instruction_data.instruction, {arguments.begin() + 1, arguments.end()});
    } catch (std::out_of_range&) {
        LOGE("AI: Invalid operation %s", operation.c_str());
        exit(EXIT_FAILURE);
    }

    return instructions;
}

std::vector<std::string> extractArguments(std::string str) {
    std::vector<std::string> ret_vec;
    std::string::iterator start_it = str.begin();
    int paren_count = 0;

    if (str.empty()) {
        return {};
    }

    if (str[0] != '(') {
        start_it = std::find(str.begin(), str.end(), ' ');

        ret_vec.push_back({str.begin(), start_it});
    }

    for (std::string::iterator it = start_it; it != str.end(); it++) {
        if (*it == '(') {
            if (paren_count == 0) {
                start_it = it;
            }
            paren_count++;
        } else if (*it == ')') {
            paren_count--;
            if (paren_count == 0) {
                ret_vec.push_back(std::string(start_it + 1, it));
            }
        }
    }

    if (paren_count != 0) {
        throw std::invalid_argument("AI condition parse error, unmatched parentheses");
    }

    return ret_vec;
}

std::vector<std::string> tokenizeString(std::string str) {
    std::vector<std::string> ret_vec;

    std::stringstream ss{str};

    std::string word;
    while (std::getline(ss, word, ' ')) {
        ret_vec.push_back(word);
    }

    return ret_vec;
}

} // ai_utils

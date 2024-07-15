#include "program.h"

#include "components/actions/common.h" // For string_action_map

#include "utils/log.h"

#include <sstream>
#include <algorithm>
#include <regex>

#include "lexer.h"
#include "parser.h"

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

    program_out.compile(ast.operation);

    return program_out;
}

void Program::compile(scripting::Operation oper) {
    // Need to evaluate arguments first
    switch (oper.instruction) {
        case scripting::Instruction::INT:
            program_.push_back(static_cast<int>(oper.instruction));
            program_.push_back(std::get<int>(oper.data));
            break;
        case scripting::Instruction::BOOL:
            program_.push_back(static_cast<int>(oper.instruction));
            program_.push_back(std::get<bool>(oper.data) ? scripting::Bool::TRUE : scripting::Bool::FALSE);
            break;
        case scripting::Instruction::STRING:
            program_.push_back(static_cast<int>(oper.instruction));

            // Add string to literals and push reference
            strings_.insert({string_id_counter_, std::get<std::string>(oper.data)});
            program_.push_back(string_id_counter_);
            string_id_counter_++;
            break;
        case scripting::Instruction::FLOAT:
            program_.push_back(static_cast<int>(oper.instruction));

            // Store float and push reference
            floats_.insert({float_id_counter_, std::get<double>(oper.data)});
            program_.push_back(float_id_counter_);
            float_id_counter_++;
            break;
        case scripting::Instruction::ACTION:
            program_.push_back(static_cast<int>(oper.instruction));
            program_.push_back(std::get<int>(oper.arguments[0].data));
            break;
        case scripting::Instruction::ENABLE_ACTION:
            program_.push_back(static_cast<int>(oper.instruction));
            program_.push_back(std::get<int>(oper.arguments[0].data));
            break;
        case scripting::Instruction::DISABLE_ACTION:
            program_.push_back(static_cast<int>(oper.instruction));
            program_.push_back(std::get<int>(oper.arguments[0].data));
            break;
        case scripting::Instruction::IF:
        {
            // Push condition
            compile(oper.arguments[0]);

            // Push IF statement
            program_.push_back(static_cast<int>(scripting::Instruction::IF));

            // Ignore THEN

            // Push body
            compile(oper.arguments[2]);

            break;
        }
        default:
            // Push arguments
            for (auto arg : oper.arguments) {
                compile(arg);
            }

            // Push original operation
            program_.push_back(static_cast<int>(oper.instruction));

            break;
    }
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

#include "parser.h"

#include <stdexcept>

#include "components/actions/common.h" // For string_action_map

#include "utils/log.h"

using scripting::Instruction;
using scripting::Type;
using scripting::Operation;
using scripting::Token;
using scripting::TokenType;

Parser::ParsedAST Parser::generateAST(std::vector<Token> lexed_tokens) {
    ParsedAST ast;

    if (lexed_tokens.size() < 1) {
        ast.operation = {};
        return ast;
    }

    lexed_tokens_ = lexed_tokens;
    current_token_ = lexed_tokens_.begin();

    if (current_token_->lexeme == "on_enter") {
        ast.on_enter = true;
        advance();
    } else if (current_token_->lexeme == "on_exit") {
        ast.on_exit = true;
        advance();
    }

    auto oper = parseStatement();

    ast.operation = oper;

    return ast;
}

Operation Parser::parseStatement() {
    if (current_token_ == lexed_tokens_.end()) {
        throw std::invalid_argument("Parser: Early termination of program");
    }

    Operation oper{};
    oper.instruction = scripting::Instruction::INVALID;
    oper.return_type = scripting::Type::NONE;
    oper.arguments = {};

    switch (current_token_->type) {
        case (TokenType::LEFT_PAREN):
            advance();
            oper = parseStatement();

            // Consume terminating ')' and check for errors
            advance();
            if (current_token_ == lexed_tokens_.end()) {
                throw std::invalid_argument("Parser: Early termination of program");
            } else if (current_token_->lexeme != ")") {
                throw std::invalid_argument("Expected ')'");
            }

            break;
        case (TokenType::RIGHT_PAREN):
            throw std::invalid_argument("Unexpected ')'");
            break;
        case (TokenType::INTEGER):
            oper.instruction = Instruction::INT;
            oper.return_type = Type::INT;
            oper.data = std::stoi(current_token_->lexeme);
            break;
        case (TokenType::DECIMAL):
            oper.instruction = Instruction::FLOAT;
            oper.return_type = Type::FLOAT;
            oper.data = std::stod(current_token_->lexeme);
            break;
        case (TokenType::MINUS):
            advance();
            oper = parseStatement();

            if (oper.return_type == Type::INT) {
                oper.data = 0 - std::get<int>(oper.data);
            } else if (oper.return_type == Type::FLOAT) {
                oper.data = 0.0 - std::get<double>(oper.data);
            } else {
                throw std::invalid_argument("Minus: Invalid number");
            }

            break;
        case (TokenType::STRING):
            oper.instruction = Instruction::STRING;
            oper.return_type = Type::STRING;
            oper.data = current_token_->lexeme;
            break;
        case (TokenType::IDENTIFIER):
            if (current_token_->lexeme == "true") {
                oper.instruction = Instruction::BOOL;
                oper.return_type = Type::BOOL;
                oper.data = true;
            } else if (current_token_->lexeme == "false") {
                oper.instruction = Instruction::BOOL;
                oper.return_type = Type::BOOL;
                oper.data = false;
            } else if (scripting::string_instruction_map.count(current_token_->lexeme) > 0) {
                auto inst_data = scripting::string_instruction_map.at(current_token_->lexeme);

                oper.instruction = inst_data.instruction;
                oper.return_type = inst_data.return_type;

                for (auto i : inst_data.args_return_type) {
                    advance();
                    auto argument = parseStatement();

                    // Allow void to take (and discard) any type
                    if (i != scripting::Type::VOID && argument.return_type != i) {
                        throw std::invalid_argument(std::string{"Incorrect return type for \"" + current_token_->lexeme + "\""});
                    }

                    oper.arguments.push_back(argument);
                }
            } else if (string_action_map.contains(current_token_->lexeme)) {
                scripting::InstructionData inst_data;
                oper.instruction = Instruction::ACTION_LITERAL;
                oper.return_type = scripting::Type::ACTION;
                oper.arguments = {};

                oper.data = static_cast<int>(string_action_map.at(current_token_->lexeme));
            } else {
                throw std::invalid_argument(std::string{"Parser: Unknown identifier \""} + current_token_->lexeme + "\"");
            }

            break;
        }

    return oper;
}

void Parser::advance() {
    if (current_token_ < lexed_tokens_.end()) {
        current_token_++;
    } else {
        throw std::invalid_argument("Unexpeted end of input");
    }
}

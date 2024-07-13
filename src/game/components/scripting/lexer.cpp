#include "lexer.h"

#include "components/actions/common.h"

#include <stdexcept>

#include "utils/log.h"

using scripting::Token;
using scripting::TokenType;

std::vector<Token> Lexer::tokenizeProgram(std::string program_string) {
    tokens_ = {};
    program_string_ = program_string;
    current_char_ = program_string_.begin();

    while (!isAtEnd()) {
        current_lexeme_start_ = current_char_;
        scanNextToken();
    }

    return tokens_;
}

void Lexer::scanNextToken() {
    auto c = advance();

    switch (c) {
        case '(':
            addToken(TokenType::LEFT_PAREN);
            break;
        case ')':
            addToken(TokenType::RIGHT_PAREN);
            break;
        case '\'':
            addString();
            break;
        case '-':
            if (isDigit(peekNext())) {
                addNumber();
            } else {
                addToken(TokenType::MINUS);
            }
            break;
        case ' ':
            // Skip spaces
            break;
        default:
            if (isDigit(c)) {
                addNumber();
            } else if (isAlpha(c)) {
                addIdentifier();
            } else {
                throw std::invalid_argument("Unkown token " + c);
            }
    }
}

void Lexer::addToken(TokenType type, std::string lexeme) {
    Token token;

    token.type = type;
    token.lexeme = lexeme;

    tokens_.push_back(token);
}

void Lexer::addToken(TokenType type) {
    addToken(type, std::string(current_lexeme_start_, current_char_));
}

void Lexer::addNumber() {
    while (isDigit(peekNext())) {
        advance();
    }

    // Handle decimal numbers
    if (peekNext() == '.' && isDigit(peekNext(1))) {
        advance();

        while (isDigit(peekNext())) {
            advance();
        }

        addToken(TokenType::DECIMAL);
    } else {
        addToken(TokenType::INTEGER);
    }

}

void Lexer::addString() {
    while ((peekNext() != '\'') && !isAtEnd()) {
        advance();
    }

    if (isAtEnd()) {
        throw std::invalid_argument("Unterminated string literal");
    }

    // Closing '
    advance();

    std::string literal = {current_lexeme_start_ + 1, current_char_ - 1};
    addToken(TokenType::STRING, literal);
}

void Lexer::addIdentifier() {
    while (isAlphaNumeric(peekNext())) {
        advance();
    }

    addToken(TokenType::IDENTIFIER);

}

bool Lexer::isAtEnd() {
    return current_char_ >= program_string_.end();
}

bool Lexer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_');
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

char Lexer::advance() {
    return *current_char_++;
}

bool Lexer::matchNext(char c) {
    if (isAtEnd()) {
        return false;
    }

    return *current_char_ == c;
}

char Lexer::peekNext(int offset) {
    if (current_char_ + offset >= program_string_.end()) {
        return '\0';
    }

    return *(current_char_ + offset);
}

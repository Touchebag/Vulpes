#pragma once

#include <unordered_map>

namespace scripting {

enum class TokenType {
    LEFT_PAREN,
    RIGHT_PAREN,

    MINUS,

    INTEGER,
    DECIMAL,
    STRING,

    IDENTIFIER,

    // Keywords
    IF,
    THEN,
    AND,
    OR,
    NOT,
    TRUE,
    FALSE,
};

const std::unordered_map<std::string, TokenType> string_keyword_map = {
    {"if", TokenType::IF},
    {"then", TokenType::THEN},
    {"and", TokenType::AND},
    {"or", TokenType::OR},
    {"not", TokenType::NOT},
    {"true", TokenType::TRUE},
    {"false", TokenType::FALSE},
};

struct Token {
    TokenType type;
    std::string lexeme;
};

} // scripting

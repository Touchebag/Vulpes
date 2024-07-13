#pragma once

#include <vector>
#include <string>

#include "data_structures.h"

class Lexer {
  public:
    std::vector<scripting::Token> tokenizeProgram(std::string program_string);

  private:
    void scanNextToken();

    void addToken(scripting::TokenType type);
    void addToken(scripting::TokenType type, std::string lexeme);
    void addNumber();
    void addString();
    void addIdentifier();

    bool isAtEnd();
    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);

    char advance();
    bool matchNext(char c);
    char peekNext(int offset = 0);

    std::string program_string_;

    std::string::iterator current_char_;
    std::string::iterator current_lexeme_start_;

    std::vector<scripting::Token> tokens_;
};

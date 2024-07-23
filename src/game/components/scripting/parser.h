#pragma once

#include <vector>

#include "data_structures.h"

class Parser {
  public:
    struct ParsedAST {
        // TODO Remove after on enter/exit rework
        scripting::Operation operation;
        bool on_enter = false;
        bool on_exit = false;
    };

    ParsedAST generateAST(std::vector<scripting::Token> lexed_tokens);

  private:
    std::vector<scripting::Token> lexed_tokens_;
    std::vector<scripting::Token>::iterator current_token_;

    void advance();
    scripting::Operation parseStatement();
};

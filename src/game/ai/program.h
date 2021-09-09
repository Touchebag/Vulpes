#pragma once

#include <vector>
#include <string>

#include "instructions.h"

class Program {
  public:
    static Program loadProgram(std::string str);

    const std::vector<int> getProgram();

  private:
    ai::Type translateAndStore(std::vector<std::string> lexed_input);

    std::vector<int> program_;
    std::unordered_map<int, std::string> strings_;
};


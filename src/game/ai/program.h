#pragma once

#include <vector>
#include <string>

#include "instructions.h"

class Program {
  public:
    static Program loadProgram(std::string str);
    const std::vector<int> getProgram();

    const std::string& getString(int id);
    double getFloat(int id);

  private:
    ai::Type translateAndStore(std::vector<std::string> lexed_input);

    std::vector<int> program_;

    int string_id_counter_ = 0;
    std::unordered_map<int, std::string> strings_;

    int float_id_counter_ = 0;
    std::unordered_map<int, double> floats_;
};


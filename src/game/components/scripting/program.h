#pragma once

#include <vector>
#include <string>

#include "data_structures.h"
#include "parser.h"

class Program {
  public:
    enum class MetaData {
        NONE,
        ON_ENTER,
        ON_EXIT,
    };

    static Program loadProgram(const std::string& str);
    const std::vector<int> getProgram();

    const std::string& getString(int id);
    double getFloat(int id);

    MetaData getMetaData();

  private:
    void compile(scripting::Operation oper);

    MetaData meta_data_ = MetaData::NONE;

    std::vector<int> program_;

    int string_id_counter_ = 0;
    std::unordered_map<int, std::string> strings_;

    int float_id_counter_ = 0;
    std::unordered_map<int, double> floats_;
};


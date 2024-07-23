#pragma once

#include <vector>
#include <string>
#include <memory>

#include "data_structures.h"
#include "parser.h"

class ComponentStore;

class Program {
  public:
    enum class MetaData {
        NONE,
        ON_ENTER,
        ON_EXIT,
    };

    struct ExtraInputData {
        std::shared_ptr<ComponentStore> this_components = std::make_shared<ComponentStore>();
        unsigned int frame_timer = 0;
        std::shared_ptr<scripting::VariableMap> variables = std::make_shared<scripting::VariableMap>();
    };

    static Program loadProgram(const std::string& str);
    scripting::return_types run(ExtraInputData extra_data) const;

    const std::string& getString(int id);
    double getFloat(int id);

    MetaData getMetaData();

  private:
    scripting::return_types executeInstruction(scripting::Operation oper, ExtraInputData& extra_data) const;

    MetaData meta_data_ = MetaData::NONE;
    Parser::ParsedAST ast_;

    std::vector<int> program_;

    int string_id_counter_ = 0;
    std::unordered_map<int, std::string> strings_;

    int float_id_counter_ = 0;
    std::unordered_map<int, double> floats_;
};


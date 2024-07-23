#pragma once

#include <optional>
#include <set>
#include <string>
#include <vector>

#include "components/scripting/data_structures.h"

namespace entity_editor {

class ScriptingEditor {
  public:
    void setActive();

    void drawMenu(std::vector<std::string>& script);

    static scripting::Type getExpectedType(std::vector<std::string> tokens);
    static std::string formatProgramString(std::string input);

    static std::pair<std::string, std::set<std::string>> getAutoCompletionSuggestions(const std::string& text);
    static std::string autoCompleteProgramString(std::string input);

    static const unsigned int IMGUI_BUFFER_SIZE = 500;

  private:
    bool active_ = false;

    int current_edit_ = -1;

    char text_buf_[IMGUI_BUFFER_SIZE] = {};

    std::set<std::string> completion_values_;
};

} // entity_editor

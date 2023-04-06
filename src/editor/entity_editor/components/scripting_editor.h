#pragma once

#include <optional>
#include <set>
#include <string>
#include <vector>

#include "components/scripting/instructions.h"

namespace entity_editor {

class ScriptingEditor {
  public:
    void setActive();

    void drawMenu(std::vector<std::string>& script);

    static scripting::Type getExpectedType(std::vector<std::string> tokens);
    static std::set<std::string> getAutoCompletionSuggestions(const std::string& text);

  private:
    bool active_ = false;

    int current_edit_ = -1;

    static const unsigned int IMGUI_BUFFER_SIZE = 500;
    char text_buf_[IMGUI_BUFFER_SIZE] = {};

    std::set<std::string> completion_values_;
};

} // entity_editor

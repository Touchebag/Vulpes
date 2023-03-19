#pragma once

#include <vector>
#include <string>
#include <optional>

namespace entity_editor {

class ScriptingEditor {
  public:
    void setActive();

    void drawMenu(std::vector<std::string>& script);

  private:
    bool active_ = false;

    long long unsigned int current_edit_ = 0;
    std::string edit_copy_string_ = {};
};

} // entity_editor

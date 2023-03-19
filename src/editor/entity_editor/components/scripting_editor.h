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

    int current_edit_ = -1;

    static const unsigned int IMGUI_BUFFER_SIZE = 500;
    char text_buf_[IMGUI_BUFFER_SIZE] = {};
};

} // entity_editor

#pragma once

#include "editor_environment.h"

class EditorMouse {
  public:
    static void handleMousePress(std::shared_ptr<EditorEnvironment> editor_env);
    static void handleMouseRelease(std::shared_ptr<EditorEnvironment> editor_env);

    static void handleMouseScroll(std::shared_ptr<EditorEnvironment> editor_env);
};

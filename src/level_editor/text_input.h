#pragma once

#include <string>

#include "components/rendering/rendering_text.h"

class TextInput {
  public:
    TextInput();

    void enterText(const std::string& text);

    std::string getString();

  private:
    std::string text_;

    std::shared_ptr<ComponentStore> components_ = std::make_shared<ComponentStore>();
};

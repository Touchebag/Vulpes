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

    std::shared_ptr<Transform> text_position_ = std::make_shared<Transform>();
    std::shared_ptr<RenderableText> text_renderable_ = std::make_shared<RenderableText>(text_position_);
};

#include "text_input.h"

#include "system/system.h"

TextInput::TextInput() {
    std::shared_ptr<Transform> text_position = std::make_shared<Transform>(components_);
    std::shared_ptr<RenderableText> text_renderable = std::make_shared<RenderableText>(components_);

    text_position->setPosition(50, 300);
    text_renderable->setColor(sf::Color::Green);
    text_renderable->setLayer(INT_MAX);

    components_->setComponent<Transform>(text_position);
    components_->setComponent<RenderableEntity>(text_renderable);

    System::getRender()->addEntity(text_renderable);
}

void TextInput::enterText(const std::string& text) {
    if (text == "\b") {
        text_.erase(text_.size() - 1, 1);
    } else {
        text_ += text;
    }

    if (auto renderable_text = std::dynamic_pointer_cast<RenderableText>(components_->getComponent<RenderableEntity>())) {
        renderable_text->setText(text_);
    }
}

std::string TextInput::getString() {
    return text_;
}

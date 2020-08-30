#include "text_input.h"

#include "system/system.h"

TextInput::TextInput() {
    text_position_->setPosition(50, 300);
    text_renderable_->setColor(sf::Color::Green);
    text_renderable_->setLayer(RenderableEntity::Layer::HUD);
    System::getRender()->addEntity(text_renderable_);
}

void TextInput::enterText(const std::string& text) {
    if (text == "\b") {
        text_.erase(text_.size() - 1, 1);
    } else {
        text_ += text;
    }

    text_renderable_->setText(text_);
}

std::string TextInput::getString() {
    return text_;
}

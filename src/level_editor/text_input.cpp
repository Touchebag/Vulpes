#include "text_input.h"

#include "render.h"

TextInput::TextInput() {
    text_position_->setPosition(50, 300);
    text_renderable_->setColor(sf::Color::Green);
    Render::getInstance().addEntity(text_renderable_, World::Layer::HUD);
}

void TextInput::enterText(const std::string& text) {
    text_ += text;
    text_renderable_->setText(text_);
}

std::string TextInput::getString() {
    return text_;
}

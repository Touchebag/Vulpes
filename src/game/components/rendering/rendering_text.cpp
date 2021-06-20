#include "components/rendering/rendering_text.h"

#include "utils/file.h"
#include "utils/log.h"
#include "components/component_store.h"

RenderingText::RenderingText(std::weak_ptr<ComponentStore> components) :
    Rendering(components) {

    sf::Font font;
    if (std::optional<sf::Font> tmp_font = File().loadFont("arial.ttf")) {
        font = tmp_font.value();
    } else {
        LOGE("Failed to load font");
    }

    setFont(font);
    setColor(sf::Color::Black);
    setText("");
}

void RenderingText::setFont(sf::Font font) {
    font_ = font;
    text_.setFont(font_);
}

void RenderingText::setColor(sf::Color color) {
    text_.setFillColor(color);
}

void RenderingText::setText(const std::string& text) {
    text_.setString(text);
}

void RenderingText::render(sf::RenderTarget& target, float /* frame_fraction */) {
    if (auto trans = getComponent<Transform>()) {
        text_.setPosition(static_cast<float>(trans->getX()), static_cast<float>(trans->getY()));
        target.draw(text_);
    }
}

#include "components/rendering/rendering_text.h"

#include "utils/file.h"
#include "utils/log.h"

RenderableText::RenderableText(std::weak_ptr<Transform> trans) :
    RenderableEntity(trans, {}) {

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

void RenderableText::setFont(sf::Font font) {
    font_ = font;
    text_.setFont(font_);
}

void RenderableText::setColor(sf::Color color) {
    text_.setFillColor(color);
}

void RenderableText::setText(const std::string& text) {
    text_.setString(text);
}

void RenderableText::render(sf::RenderTarget& target, float /* frame_fraction */) {
    if (auto trans = trans_.lock()) {
        text_.setPosition(static_cast<float>(trans->getX()), static_cast<float>(trans->getY()));
        target.draw(text_);
    }
}

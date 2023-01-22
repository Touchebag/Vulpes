#include "editor_view.h"

#include "utils/log.h"

namespace entity_editor {

EditorView::EditorView(sf::RenderWindow& window) :
    window_(window),
    mouse_(window) {
}

void EditorView::handleMouseEvent(sf::Event event) {
    switch (event.type) {
        case sf::Event::MouseWheelScrolled:
            break;
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Button::Left ||
                event.mouseButton.button == sf::Mouse::Button::Right) {

                mouse_.saveMousePosition();
                initial_mouse_pos_ = center_position_;
                dragging_view_ = true;
            }
            break;
        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Button::Left ||
                event.mouseButton.button == sf::Mouse::Button::Right) {

                dragging_view_ = false;
            }
            break;
        default:
            break;
    }
}

void EditorView::update() {
    if (dragging_view_) {
        std::pair<float, float> new_pos;
        auto distance = mouse_.getMouseWorldDistance();

        new_pos.first = initial_mouse_pos_.first + distance.first;
        new_pos.second = initial_mouse_pos_.second + distance.second;

        setView(new_pos, size_);
    }
}

void EditorView::setView(std::pair<float, float> pos, std::pair<float, float> size) {
    center_position_ = pos;
    size_ = size;

    auto view = window_.getView();
    view.setSize(size_.first, size_.second);
    view.setCenter(center_position_.first, center_position_.second);

    window_.setView(view);
}

} // entity_editor

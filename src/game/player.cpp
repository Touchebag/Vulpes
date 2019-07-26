#include "player.h"
#include "input_event.h"

#include <tuple>
#include <algorithm>

void Player::update() {
    // Check before decreasing
    // If previous frame was the last one (i.e. ticked down to 0) then trigger event before this frame
    if (frame_counter_-- == 0) {
        incomingEvent(state::Event::FRAME_TIMEOUT);
    }

    double x = velx_, y = vely_;

    if (!getStateProperties().movement_locked_x_) {
        if (Input::getInstance().isButtonHeld(input::button::LEFT)) {
            if (getStateProperties().touching_ground_) {
                x = -10.0;
            } else {
                x = std::max(x - 1.0, -10.0);
            }
            incomingEvent(state::Event::MOVING);

            // When moving left facing_right_ should be false even when speed is zero
            facing_right_ = x > 0.0;
        } else if (Input::getInstance().isButtonHeld(input::button::RIGHT)) {
            if (getStateProperties().touching_ground_) {
                x = 10.0;
            } else {
                x = std::min(x + 1.0, 10.0);
            }
            incomingEvent(state::Event::MOVING);

            // When moving right facing_right_ should be true even when speed is zero
            facing_right_ = x >= 0.0;
        } else {
            if (getStateProperties().touching_ground_) {
                x /= 5.0;
            }
            incomingEvent(state::Event::NO_MOVEMENT);
        }
    }

    if (!getStateProperties().movement_locked_y_) {
        // Gravity
        y += 1.0;
    }

    if (getStateProperties().touching_wall_) {
        y = std::min(y, 5.0);
    }

    if (Input::getInstance().isButtonPressed(input::button::DASH)) {
        if (getStateProperties().can_dash_) {
            x = 50.0 * (facing_right_ ? 1.0 : -1.0);
            y = 0.0;
            incomingEvent(state::Event::DASHING);
        }
    }

    if (Input::getInstance().isButtonPressed(input::button::JUMP)) {
        if (getStateProperties().touching_wall_) {
            facing_right_ = !facing_right_;
            int dir = facing_right_ ? 1.0 : -1.0;
            x = 10.0 * dir;
            y = -20.0;

            incomingEvent(state::Event::JUMPING);
        } else if (getStateProperties().can_jump_) {
            y = -20.0;
            incomingEvent(state::Event::JUMPING);
        }
    }

    y = std::max(std::min(y, 20.0), -20.0);
    auto max_movement = getMaximumMovement(x, y, getAbsHitbox());

    if (static_cast<int>(max_movement.second) < y) {
        incomingEvent(state::Event::TOUCHING_FLOOR);
    } else if (static_cast<int>(max_movement.first) != static_cast<int>(x)) {
        incomingEvent(state::Event::TOUCHING_WALL);
    } else if (static_cast<int>(max_movement.second) > 0.0) {
        incomingEvent(state::Event::FALLING);
    }

    move(max_movement.first, max_movement.second, trans_);

    updateState();
    auto sprite_rect = getSpriteRect(getCurrentSprite());
    setTextureCoords(sprite_rect);
}

void Player::loadFromJson(nlohmann::json j) {
    BaseEntity::loadFromJson(j);

    loadSpriteMap(j["sprite_map"].get<std::string>());

    auto sprite_rect = getSpriteRect(getCurrentSprite());
    setTextureCoords(sprite_rect);

    incomingEvent(state::Event::START);
}

std::optional<nlohmann::json> Player::outputToJson() {
    auto j = BaseEntity::outputToJson();

    if (j) {
        j.value()["sprite_map"] = sprite_map_;
    }

    return j;
}

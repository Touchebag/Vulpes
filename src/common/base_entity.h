#pragma once

#include <SFML/Graphics.hpp>
#include <json.hpp>

#include "transform.h"
#include "hitbox.h"
#include "utils/common.h"

class BaseEntity {
public:
    bool loadTexture(std::string file_path);
    virtual void update();

    // TODO Move to constructor?
    void setPosition(util::X abs_x, util::Y abs_y);
    sf::Vector2i getPosition();
    void setHitbox(util::X width, util::Y height);
    Hitbox getAbsHitbox();

    void loadFromJson(nlohmann::json j);
    void render(sf::RenderWindow& window);

    void setTextureCoords(std::pair<util::Point, util::Point>);

    virtual ~BaseEntity() {};

protected:
    Transform trans_;
    Hitbox hitbox_;

    // Used for mirroring sprites
    bool facing_right_ = true;

private:
    sf::Texture texture_;
    sf::Sprite sprite_;
};
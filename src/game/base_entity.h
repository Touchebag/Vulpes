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
    void setPosition(int abs_x, int abs_y);
    sf::Vector2i getPosition();
    void setHitbox(int width, int height);
    Hitbox getHitbox();
    Hitbox getAbsHitbox();

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

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

    // Needed for level editor
    std::string texture_name_;
};

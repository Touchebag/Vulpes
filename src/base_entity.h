#pragma once

#include <SFML/Graphics.hpp>

class BaseEntity {
public:
    BaseEntity();

    virtual void update();

    void setPosiition(float abs_x, float abs_y);

    void render(sf::RenderWindow& window);

    void move(float rel_x,float rel_y);

private:
    sf::Texture texture_;
    sf::Sprite sprite_;
};

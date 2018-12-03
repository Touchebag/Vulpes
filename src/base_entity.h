#include <SFML/Graphics.hpp>

class BaseEntity {
public:
    BaseEntity();

    void update();

    sf::Sprite sprite_;

private:
    sf::Texture texture_;
};

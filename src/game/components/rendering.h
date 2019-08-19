#pragma once

#include <memory>
#include <json.hpp>
#include <SFML/Graphics.hpp>

#include "transform.h"

class RenderableEntity {
  public:
    RenderableEntity(std::weak_ptr<Transform> trans);

    bool loadTexture(std::string file_path);

    void loadFromJson(nlohmann::json j);
    std::optional<nlohmann::json> outputToJson();

    void setTextureCoords(int pos_x, int pos_y, int width, int height);

    void render(sf::RenderWindow& window);

    // Used for mirroring sprites
    bool facing_right_ = true;

  private:
    std::weak_ptr<Transform> trans_;

    sf::Texture texture_;
    sf::Sprite sprite_;

    float scale_ = 1.0;

    // Needed for level editor
    std::string texture_name_;
};

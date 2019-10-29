#pragma once

#include <memory>
#include <json.hpp>
#include <SFML/Graphics.hpp>

#include "components/transform.h"

class RenderableEntity {
  public:
    RenderableEntity(std::weak_ptr<Transform> trans);
    virtual ~RenderableEntity() = default;

    virtual bool loadTexture(std::string file_path);

    virtual void loadFromJson(nlohmann::json j);
    virtual std::optional<nlohmann::json> outputToJson();

    virtual void recalculateTextureRect(int width, int height);
    virtual void setTiling(bool tiling_x, bool tiling_y, int width, int height);

    virtual void setTextureCoords(int pos_x, int pos_y, int width, int height);

    virtual void render(sf::RenderWindow& window);

    // Used for mirroring sprites
    bool facing_right_ = true;

    bool tiling_x_ = true;
    bool tiling_y_ = true;
  protected:
    std::weak_ptr<Transform> trans_;

  private:
    sf::Texture texture_;
    sf::Sprite sprite_;

    float scale_ = 1.0;

    // Needed for level editor
    std::string texture_name_;

    sf::IntRect original_texture_rect_;
};

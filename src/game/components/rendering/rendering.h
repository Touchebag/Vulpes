#pragma once

#include <memory>
#include <json.hpp>
#include <SFML/Graphics.hpp>

#include "components/transform.h"
#include "components/movement.h"

class RenderableEntity {
  public:
    RenderableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<MovableEntity> movable);
    virtual ~RenderableEntity() = default;

    virtual bool loadTexture(std::string file_path);

    virtual void reloadFromJson(nlohmann::json j);
    virtual std::optional<nlohmann::json> outputToJson();

    virtual void recalculateTextureRect();
    virtual void setTiling(bool tiling_x, bool tiling_y);

    virtual void setTextureCoords(int pos_x, int pos_y, int width, int height);

    virtual void setSize(int width, int height);

    virtual void render(sf::RenderWindow& window);

    bool tiling_x_ = true;
    bool tiling_y_ = true;

  protected:
    std::weak_ptr<Transform> trans_;
    std::weak_ptr<MovableEntity> movable_;

  private:
    sf::Texture texture_;
    sf::Sprite sprite_;

    int height_ = 0;
    int width_ = 0;

    // Needed for level editor
    std::string texture_name_;

    sf::IntRect original_texture_rect_;
};

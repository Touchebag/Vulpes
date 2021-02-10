#pragma once

#include <memory>
#include <json.hpp>
#include <SFML/Graphics.hpp>

#include "components/transform.h"
#include "components/movement.h"

class RenderableEntity : ComponentWithFile {
  public:
    RenderableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<MovableEntity> movable);
    virtual ~RenderableEntity() = default;

    virtual bool loadTexture(std::string file_path, File file_instance);

    virtual void reloadFromJson(nlohmann::json j) override ;
    virtual void reloadFromJson(nlohmann::json j, File file_instance) override ;
    virtual std::optional<nlohmann::json> outputToJson() override ;

    virtual void recalculateTextureRect();
    virtual void setTiling(bool tiling_x, bool tiling_y);

    virtual void setTextureCoords(int pos_x, int pos_y, int width, int height);

    virtual void setSize(int width, int height);
    virtual std::pair<int, int> getSize();
    virtual std::pair<float, float> getScaledSize();

    virtual void setColor(sf::Color color);
    virtual void clearColor();

    virtual void render(sf::RenderWindow& window, float frame_fraction = 0.0f);

    virtual void update() override;

    int getLayer();
    void setLayer(int layer);

    void loadShader(std::string shader_name);

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

    int layer_ = 0;

    // Needed for level editor
    std::string texture_name_;

    // Needed for texture loading
    std::string entity_name_;

    sf::IntRect original_texture_rect_;

    std::shared_ptr<sf::Shader> shader_;
};

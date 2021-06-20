#pragma once

#include <memory>
#include <json.hpp>
#include <SFML/Graphics.hpp>

#include "components/transform.h"
#include "components/movement.h"

class Rendering : public Component {
  public:
    Rendering(std::weak_ptr<ComponentStore> components);
    virtual ~Rendering() = default;

    bool loadTexture(std::string file_path, File file_instance);

    static std::shared_ptr<Rendering> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    virtual std::optional<nlohmann::json> outputToJson() override ;

    void setTexture(std::shared_ptr<sf::Texture> texture);
    void recalculateTextureRect();

    std::pair<int, int> getTiling();
    void setTiling(int tiling_x, int tiling_y);

    void setTextureCoords(int pos_x, int pos_y, int width, int height);

    void setSize(int width, int height);
    std::pair<int, int> getSize();
    std::pair<float, float> getScaledSize();

    void setScale(float x_scale, float y_scale);

    virtual void setColor(sf::Color color);
    virtual void clearColor();

    virtual void render(sf::RenderTarget& target, float frame_fraction = 0.0f);

    void update() override;

    int getLayer();
    void setLayer(int layer);

    void loadShader(std::string shader_name);

    // Number of repeated sprites
    // 0 = infinite tiling
    int tiling_x_ = 0;
    int tiling_y_ = 0;

  private:
    std::shared_ptr<sf::Texture> texture_;
    sf::Sprite sprite_;

    int height_ = 0;
    int width_ = 0;

    int layer_ = 0;

    float x_scale_ = 1.0;
    float y_scale_ = 1.0;

    // Needed for level editor
    std::string texture_name_;

    // Needed for texture loading
    std::string entity_name_;

    sf::IntRect original_texture_rect_;

    std::shared_ptr<sf::Shader> shader_;
};

#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

#include "components/transform.h"
#include "components/movement.h"
#include "system/shader_handle.h"

class Rendering : public Component {
  public:
    explicit Rendering(std::weak_ptr<ComponentStore> components);
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
    std::pair<double, double> getScaledSize();

    void setScale(double x_scale, double y_scale);

    void setOffset(int x_offset, int y_offset);

    virtual sf::Color getColor();
    virtual void setColor(sf::Color color);
    virtual void clearColor();

    virtual void render(sf::RenderTarget& target, double frame_fraction = 0.0f);

    void update() override;

    int getLayer();
    void setLayer(int layer);

    std::vector<std::shared_ptr<ShaderHandle>> getShaders();

    // Number of repeated sprites
    // 0 = infinite tiling
    int tiling_x_ = 1;
    int tiling_y_ = 1;

  private:
    std::shared_ptr<sf::Texture> texture_;
    sf::Sprite sprite_;

    int height_ = 0;
    int width_ = 0;

    int layer_ = 0;

    double flip_x_ = 1.0;
    double flip_y_ = 1.0;

    double x_scale_ = 1.0;
    double y_scale_ = 1.0;

    int x_offset_ = 0;
    int y_offset_ = 0;

    void loadShader(nlohmann::json j);

    // Needed for level editor
    std::string texture_name_;

    // Needed for texture loading
    std::string entity_name_;

    sf::IntRect original_texture_rect_;

    std::vector<std::shared_ptr<ShaderHandle>> shaders_;

    std::shared_ptr<sf::RenderTexture> front_render_texture_;
    std::shared_ptr<sf::RenderTexture> back_render_texture_;
};

#pragma once

#include <memory>
#include <json.hpp>
#include <SFML/Graphics.hpp>

#include "components/transform.h"
#include "components/movement.h"

class RenderableEntity {
  public:
    enum class Layer {
        BACKGROUND,

        // Counting outwards from MAIN
        BG_3,
        BG_2,
        BG_1,

        MAIN_BG,
        MAIN,
        MAIN_FG,

        // Counting outwards from MAIN
        FG_1,
        FG_2,
        FG_3,

        // HUD is treated differently
        HUD,
        MAX_LAYERS = HUD
    };

    RenderableEntity(std::weak_ptr<Transform> trans, std::weak_ptr<MovableEntity> movable);
    virtual ~RenderableEntity() = default;

    virtual bool loadTexture(std::string file_path);

    virtual void reloadFromJson(nlohmann::json j);
    virtual std::optional<nlohmann::json> outputToJson();

    virtual void recalculateTextureRect();
    virtual void setTiling(bool tiling_x, bool tiling_y);

    virtual void setTextureCoords(int pos_x, int pos_y, int width, int height);

    virtual void setSize(int width, int height);
    virtual std::pair<int, int> getSize();
    virtual std::pair<float, float> getScaledSize();

    virtual void setColor(sf::Color color);
    virtual void clearColor();

    virtual void render(sf::RenderWindow& window);

    Layer getLayer();
    void setLayer(Layer layer);

    static std::optional<std::string> getLayerString(Layer layer);

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

    Layer layer_ = Layer::MAIN;

    // Needed for level editor
    std::string texture_name_;

    // Needed for texture loading
    std::string entity_name_;

    sf::IntRect original_texture_rect_;

    std::shared_ptr<sf::Shader> shader_;
};

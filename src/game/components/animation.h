#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <json.hpp>

#include "utils/common.h"
#include "components/rendering/rendering.h"

// This class keeps the sprite sheet coordinates of the currently loaded texture
class Animation : public Component {
  public:
    Animation(std::weak_ptr<ComponentStore> components);

    void setFrameList(std::string animation_name);

    // Returns a pair of the top-left and bottom-right points of current texture

    static std::shared_ptr<Animation> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    // Returns true if current animation has just looped
    bool hasAnimationLooped();

  private:
    struct AnimationFrameData {
        std::string texture;
        util::Rectangle sprite_rectangle;
        float x_scale = 1.0;
        float y_scale = 1.0;
    };

    AnimationFrameData getFrameData();

    std::unordered_map<std::string, AnimationFrameData> loadSpriteMap(File file_instance, std::string file);
    void loadTexture(File file_instance, std::string file);

    std::shared_ptr<std::vector<AnimationFrameData>> loadAnimationFromJson(
            const nlohmann::json& j,
            const std::unordered_map<std::string, AnimationFrameData>& sprite_map);

    void setRenderTexture();

    // Data
    std::unordered_map<std::string, std::shared_ptr<std::vector<AnimationFrameData>>> sprite_sheet_map_;
    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures_;

    std::optional<nlohmann::json> original_frame_list_;

    std::shared_ptr<std::vector<AnimationFrameData>> current_frame_list_;
    int current_frame_ = 0;
};

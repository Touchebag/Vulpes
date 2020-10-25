#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <json.hpp>

#include "utils/common.h"
#include "components/rendering/rendering.h"

// This class keeps the sprite sheet coordinates of the currently loaded texture
class AnimatedEntity : public Component {
  public:
    AnimatedEntity(std::weak_ptr<RenderableEntity> renderableEntity);

    void setFrameList(std::vector<std::string> frame_list);

    // Returns a pair of the top-left and bottom-right points of current texture
    util::Rectangle getSpriteRect();

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    // Returns true if current animation has just looped
    bool hasAnimationLooped();

  private:
    void loadSpriteMap(const std::string& path);

    void setRenderTexture();

    std::weak_ptr<RenderableEntity> renderableEntity_;

    std::unordered_map<std::string, util::Rectangle> sprite_sheet_map_;

    std::vector<std::string> frame_list_;
    // Needed to be able to keep original values when saving entity
    // States will overwrite active frame list
    std::vector<std::string> original_frame_list_;

    int current_frame_ = 0;
};

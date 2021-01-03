#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <json.hpp>

#include "utils/common.h"
#include "components/rendering/rendering.h"

// This class keeps the sprite sheet coordinates of the currently loaded texture
class AnimatedEntity : public ComponentWithFile {
  public:
    AnimatedEntity(std::weak_ptr<RenderableEntity> renderableEntity);

    void setFrameList(std::string animation_name);

    // Returns a pair of the top-left and bottom-right points of current texture
    util::Rectangle getSpriteRect();

    void reloadFromJson(nlohmann::json j) override;
    void reloadFromJson(nlohmann::json j, File file_instance) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    // Returns true if current animation has just looped
    bool hasAnimationLooped();

  private:
    std::unordered_map<std::string, util::Rectangle> loadSpriteMap(File file_instance);
    void loadFrameList(File file_instance,
                       const std::unordered_map<std::string, std::vector<std::string>>& list_frame_map);

    void setRenderTexture();

    std::weak_ptr<RenderableEntity> renderableEntity_;

    std::unordered_map<std::string, std::shared_ptr<std::vector<util::Rectangle>>> sprite_sheet_map_;

    std::optional<std::vector<std::string>> original_frame_list_;

    std::shared_ptr<std::vector<util::Rectangle>> current_frame_list_;
    int current_frame_ = 0;
};

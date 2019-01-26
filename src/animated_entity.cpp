#include "animated_entity.h"

typedef std::unordered_map<std::string, std::pair<util::Point, util::Point>> point_map;

void AnimatedEntity::set_sprite_map(point_map& sprite_sheet) {
    sprite_sheet_map_ = std::move(sprite_sheet);
}

std::pair<util::Point, util::Point> AnimatedEntity::get_sprite_rect(std::string sprite) {
    // TODO This is origin and size, not two coordinates
    // Change type to reflect that
    return sprite_sheet_map_.at(sprite);
}

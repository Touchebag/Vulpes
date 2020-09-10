#pragma once

#include <memory>

#include "base_entity.h"
#include "system/world.h"

namespace editor_common {

std::shared_ptr<BaseEntity> makeHudText(std::pair<int, int> position);

} // editor_common

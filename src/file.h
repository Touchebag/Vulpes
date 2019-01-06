#pragma once

#include <optional>

#include "json.hpp"

namespace file {

std::optional<nlohmann::json> loadJson(std::string filepath);

} // file

#pragma once

#include <optional>

#include "json.hpp"

namespace file {

std::optional<nlohmann::json> loadJson(std::string filepath);

bool storeJson(std::string filepath, nlohmann::json world);

} // file

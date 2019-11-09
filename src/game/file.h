#pragma once

#include <optional>
#include <fstream>
#include <SFML/Graphics.hpp>

#include "json.hpp"

namespace file {

std::ifstream openFileForInput(std::string filepath);

std::optional<nlohmann::json> loadJson(std::string filepath);
std::optional<nlohmann::json> loadEntityFromFile(std::string filepath);

bool storeJson(std::string filepath, nlohmann::json world);

std::optional<sf::Texture> loadTexture(std::string filepath);
std::optional<sf::Font> loadFont(std::string filepath);

} // file

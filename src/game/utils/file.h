#pragma once

#include <optional>
#include <fstream>
#include <SFML/Graphics.hpp>

#include "json.hpp"

class File {
  public:
    static std::optional<nlohmann::json> loadEntityFromFile(std::string filepath);
    static std::optional<nlohmann::json> loadRoom(std::string filepath);
    static std::optional<nlohmann::json> loadStates(std::string filepath);
    static std::optional<sf::Texture> loadTexture(std::string filepath);
    static std::optional<sf::Font> loadFont(std::string filepath);

    static std::ifstream openSpriteMapFile(std::string filepath);

    static bool writeJsonToFile(std::string filepath, nlohmann::json world);
  private:
    static std::optional<nlohmann::json> loadJson(std::string filepath);

    static std::ifstream openFileForInput(std::string filepath);
    static std::ofstream openFileForOutput(std::string filepath);
};

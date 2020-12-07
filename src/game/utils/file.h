#pragma once

#include <optional>
#include <fstream>
#include <SFML/Graphics.hpp>

#include "json.hpp"

class File {
  public:
    static std::optional<nlohmann::json> loadEntityFromFile(std::string entity_name);
    static std::optional<nlohmann::json> loadRoom(std::string filepath);
    static std::optional<nlohmann::json> loadStates(std::string entity_name);
    static std::optional<nlohmann::json> loadAiBehavior(std::string entity_name);
    static std::optional<nlohmann::json> loadAnimations(std::string entity_name);
    static std::optional<sf::Texture> loadTexture(std::string file, std::string entity_name = "");
    static std::optional<sf::Font> loadFont(std::string filepath);

    static std::shared_ptr<sf::Shader> loadShader(std::string filepath);

    static std::ifstream openSpriteMapFile(std::string entity_name);

    static bool writeJsonToFile(std::string filepath, nlohmann::json world);

    // This will append a suffix to a file name iff it is not already there
    static std::string appendSuffix(std::string filepath, const std::string& suffix);
  private:
    static std::optional<nlohmann::json> loadJson(std::string filepath);

    static std::ifstream openFileForInput(std::string filepath);
    static std::ofstream openFileForOutput(std::string filepath);
};

#pragma once

#include <optional>
#include <fstream>
#include <SFML/Graphics.hpp>

#include "json.hpp"

class File {
  public:
    File() = default;
    File(std::string ns);

    File(const File&) = default;
    File(File&&) = default;
    File& operator=(const File&) = default;
    File& operator=(File&&) = default;

    std::optional<nlohmann::json> loadEntityFromFile();
    std::optional<nlohmann::json> loadAiBehavior();
    std::optional<nlohmann::json> loadStates();
    std::optional<nlohmann::json> loadAnimations();
    std::optional<sf::Texture> loadTexture(std::string file);
    std::optional<sf::Font> loadFont(std::string filepath);
    std::optional<nlohmann::json> loadRoom(std::string filepath);
    std::optional<nlohmann::json> loadRoomTemplate(std::string filepath);

    std::ifstream openSpriteMapFile();

    std::shared_ptr<sf::Shader> loadShader(std::string filepath);

    bool writeJsonToFile(std::string filepath, nlohmann::json world);

    // This will append a suffix to a file name iff it is not already there
    static std::string appendSuffix(std::string filepath, const std::string& suffix);
  private:

    std::optional<nlohmann::json> loadJson(std::string filepath);

    std::ifstream openFileForInput(std::string filepath);
    std::ofstream openFileForOutput(std::string filepath);

    std::string current_namespace_ = "_default";
    bool default_constructed_ = true;
};

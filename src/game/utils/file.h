#pragma once

#include <optional>
#include <fstream>
#include <filesystem>
#include <SFML/Graphics.hpp>

#include "nlohmann/json.hpp"

class File {
  public:
    File() = default;
    explicit File(const std::string& ns);

    File(const File&) = default;
    File(File&&) = default;
    File& operator=(const File&) = default;
    File& operator=(File&&) = default;

    std::filesystem::directory_iterator getDirContents(const std::filesystem::path path);

    std::optional<nlohmann::json> loadEntityFromFile();
    std::optional<nlohmann::json> loadAiBehavior();
    std::optional<nlohmann::json> loadStates();
    std::optional<nlohmann::json> loadAnimations();
    std::optional<sf::Texture> loadTexture(std::filesystem::path file);
    std::optional<sf::Font> loadFont(std::filesystem::path filepath);
    std::optional<nlohmann::json> loadRoom(std::filesystem::path filepath);
    std::optional<nlohmann::json> loadRoomTemplate(std::filesystem::path filepath);
    std::optional<nlohmann::json> loadCutscene(std::filesystem::path filepath);

    nlohmann::json loadSaveFile();
    void writeSaveFile(nlohmann::json j);

    std::ifstream openSpriteMapFile(const std::filesystem::path file);

    std::shared_ptr<sf::Shader> loadShader(std::filesystem::path filepath);

    bool writeJsonToFile(std::filesystem::path filepath, nlohmann::json world);

  private:

    std::optional<nlohmann::json> loadJson(std::filesystem::path filepath);

    std::ifstream openFileForInput(std::filesystem::path filepath);
    std::ofstream openFileForOutput(std::filesystem::path filepath);

    std::string current_namespace_ = "_default";
    bool default_constructed_ = true;
};

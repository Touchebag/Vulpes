#pragma once

#include <optional>
#include <fstream>
#include <filesystem>
#include <stack>

#include <SFML/Graphics.hpp>

#include "nlohmann/json.hpp"

class File {
  public:
    static std::filesystem::directory_iterator getDirContents(const std::filesystem::path path);

    static std::optional<nlohmann::json> loadEntityFromFile();
    static std::optional<nlohmann::json> loadAiBehavior();
    static std::optional<nlohmann::json> loadStates();
    static std::optional<nlohmann::json> loadAnimations();
    static std::optional<sf::Texture> loadTexture(std::filesystem::path file);
    static std::optional<sf::Font> loadFont(std::filesystem::path filepath);
    static std::optional<nlohmann::json> loadRoom(std::filesystem::path filepath);
    static std::optional<nlohmann::json> loadRoomTemplate(std::filesystem::path filepath);
    static std::optional<nlohmann::json> loadCutscene(std::filesystem::path filepath);

    static std::filesystem::path getEntityDir();

    static nlohmann::json loadSaveFile();
    static void writeSaveFile(nlohmann::json j);

    static std::ifstream openSpriteMapFile(const std::filesystem::path file);

    static std::shared_ptr<sf::Shader> loadShader(std::filesystem::path filepath);

    static bool writeJsonToFile(std::filesystem::path filepath, nlohmann::json world);

    static void pushDirectory(std::filesystem::path dir);
    static void popDirectory();

  private:
    static std::stack<std::filesystem::path> current_directory_;

    static std::filesystem::path getEntityPrefixPath();

    static std::optional<nlohmann::json> loadJson(std::filesystem::path filepath);

    static std::ifstream openFileForInput(std::filesystem::path filepath);
    static std::ofstream openFileForOutput(std::filesystem::path filepath);
};

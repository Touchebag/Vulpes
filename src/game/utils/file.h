#pragma once

#include <optional>
#include <fstream>
#include <filesystem>
#include <stack>

#include <SFML/Graphics.hpp>

#include "nlohmann/json.hpp"

class File {
  public:
    class DirectoryScope {
      public:
        explicit DirectoryScope(std::filesystem::path dir) : dir_(dir) {
        }

        ~DirectoryScope() {
            // Allow creation of empty/placeholder scopes
            if (dir_.string() != "") {
                File::popDirectory(dir_);
            }
        }

        DirectoryScope(const DirectoryScope&) = delete;
        DirectoryScope& operator=(const DirectoryScope&) = delete;

        DirectoryScope& operator=(DirectoryScope&&) = default;

      private:
        std::filesystem::path dir_;
    };

    static std::filesystem::directory_iterator getDirContents(const std::filesystem::path path);

    static std::optional<nlohmann::json> loadEntityFromFile();
    static std::optional<nlohmann::json> loadAiBehavior();
    static std::optional<nlohmann::json> loadStates();
    static std::optional<nlohmann::json> loadAnimations();
    static std::optional<sf::Texture> loadTexture(std::filesystem::path file);
    static std::optional<sf::Font> loadFont(std::filesystem::path filepath);
    static std::optional<nlohmann::json> loadRoom(std::filesystem::path filepath);
    static std::optional<nlohmann::json> loadRoomTemplate(std::filesystem::path filepath);
    static std::optional<nlohmann::json> loadCutscene();

    static std::filesystem::path getEntityDir();
    static std::filesystem::path getCutsceneDir();

    static std::filesystem::path getFullPath(std::filesystem::path path);
    static bool isInRoot();

    static nlohmann::json loadSaveFile();
    static void writeSaveFile(nlohmann::json j);

    static std::ifstream openSpriteMapFile(const std::filesystem::path file);

    static std::shared_ptr<sf::Shader> loadShader(std::filesystem::path filepath);

    static bool writeJsonToFile(std::filesystem::path filepath, nlohmann::json world);

    [[nodiscard]] static DirectoryScope pushDirectory(std::filesystem::path dir);
    static std::filesystem::path getCurrentDirectory();

  private:
    static void popDirectory(std::filesystem::path);

    static std::stack<std::filesystem::path> current_directory_;

    static std::optional<nlohmann::json> loadJson(std::filesystem::path filepath);

    static std::ifstream openFileForInput(std::filesystem::path filepath);
    static std::ofstream openFileForOutput(std::filesystem::path filepath);
};

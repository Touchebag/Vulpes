#include "file.h"
#include "log.h"

const std::string ASSET_DIR = "assets";
const std::string ENTITY_DIR = "entities";

std::ifstream file::openFileForInput(std::string filepath) {
    filepath = ASSET_DIR + "/" + filepath;

    std::ifstream fs(filepath);

    if (!fs) {
        LOGE("Failed to open file %s", filepath.c_str());
        exit(1);
    }

    return fs;
}

std::optional<nlohmann::json> file::loadJson(std::string filepath) {
    filepath = ASSET_DIR + "/" + filepath;

    try {
        std::ifstream ifs(filepath);
        return nlohmann::json::parse(ifs);
    } catch (nlohmann::json::parse_error& e) {
        LOGE("Unable to load file %s: %s", filepath.c_str(), e.what());
        return {};
    }
}

std::optional<nlohmann::json> file::loadEntityFromFile(std::string filepath) {
    return loadJson(ENTITY_DIR + "/" + filepath);
}

bool file::storeJson(std::string filepath, nlohmann::json j) {
    filepath = ASSET_DIR + "/" + filepath;

    try {
        std::ofstream ofs(filepath);
        ofs << j.dump(4);
        return true;
    } catch (nlohmann::json::parse_error& e) {
        LOGE("Unable to save file %s: %s", filepath.c_str(), e.what());
        return false;
    }
}

std::optional<sf::Texture> file::loadTexture(std::string filepath) {
    sf::Texture texture;
    if (!texture.loadFromFile(ASSET_DIR + "/" + filepath)) {
        return std::nullopt;
    } else {
        return {texture};
    }
}

std::optional<sf::Font> file::loadFont(std::string filepath) {
    sf::Font font;
    if (!font.loadFromFile(ASSET_DIR + "/" + filepath)) {
        return std::nullopt;
    } else {
        return {font};
    }
}

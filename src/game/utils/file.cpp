#include "file.h"
#include "utils/log.h"

const std::string ASSET_DIR = "assets";
const std::string ENTITY_DIR = "entities";
const std::string ROOM_DIR = "rooms";
const std::string STATE_DIR = "states";
const std::string TEXTURE_DIR = "textures";
const std::string SPRITE_MAP_DIR = "sprite_maps";
const std::string FONT_DIR = "fonts";

std::ifstream File::openFileForInput(std::string filepath) {
    filepath = ASSET_DIR + "/" + filepath;

    std::ifstream fs(filepath);

    if (!fs) {
        LOGE("Failed to open file %s for input", filepath.c_str());
        exit(1);
    }

    return fs;
}

std::ofstream File::openFileForOutput(std::string filepath) {
    filepath = ASSET_DIR + "/" + filepath;

    std::ofstream fs(filepath);

    if (!fs) {
        LOGE("Failed to open file %s for output", filepath.c_str());
        exit(1);
    }

    return fs;
}

std::optional<nlohmann::json> File::loadJson(std::string filepath) {
    try {
        return nlohmann::json::parse(openFileForInput(filepath));
    } catch (nlohmann::json::parse_error& e) {
        LOGE("Unable to load file %s: %s", filepath.c_str(), e.what());
        return {};
    }
}

std::optional<nlohmann::json> File::loadEntityFromFile(std::string filepath) {
    return loadJson(ENTITY_DIR + "/" + filepath);
}

std::ifstream File::openSpriteMapFile(std::string filepath) {
    return openFileForInput(SPRITE_MAP_DIR + "/" + filepath);
}

bool File::writeJsonToFile(std::string filepath, nlohmann::json j) {
    try {
        openFileForOutput(ROOM_DIR + "/" + filepath) << j.dump(4);
        return true;
    } catch (nlohmann::json::parse_error& e) {
        LOGE("Unable to save file %s: %s", filepath.c_str(), e.what());
        return false;
    }
}

std::optional<nlohmann::json> File::loadRoom(std::string filepath) {
    return loadJson(ROOM_DIR + "/" + filepath);
}

std::optional<sf::Texture> File::loadTexture(std::string filepath) {
    sf::Texture texture;
    if (!texture.loadFromFile(ASSET_DIR + "/" + TEXTURE_DIR + "/" + filepath)) {
        return std::nullopt;
    } else {
        return {texture};
    }
}

std::optional<sf::Font> File::loadFont(std::string filepath) {
    sf::Font font;
    if (!font.loadFromFile(ASSET_DIR + "/" + FONT_DIR + "/" + filepath)) {
        return std::nullopt;
    } else {
        return {font};
    }
}
std::optional<nlohmann::json> File::loadStates(std::string filepath) {
    return loadJson(STATE_DIR + "/" + filepath);
}
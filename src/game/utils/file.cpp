#include "file.h"
#include "utils/log.h"

const std::string ASSET_DIR = "assets";
const std::string ENTITY_DIR = "entities";
const std::string ROOM_DIR = "rooms";
const std::string ROOM_TEMPLATE_DIR = "_templates";
const std::string TEXTURE_DIR = "textures";
const std::string FONT_DIR = "fonts";
const std::string SHADER_DIR = "shaders";

const std::string ENTITY_FILE = "entity";
const std::string AI_FILE = "ai";
const std::string STATE_FILE = "state";
const std::string ANIMATIONS_FILE = "animations";

namespace {

bool stringEndsWith(const std::string &full_string, const std::string& suffix) {
    if (full_string.length() >= suffix.length()) {
        return (full_string.compare(full_string.length() - suffix.length(), suffix.length(), suffix) == 0);
    } else {
        return false;
    }
}

} // namespace

File::File(std::string ns) :
    current_namespace_(ns) ,
    default_constructed_(false) {
}

std::string File::appendSuffix(std::string filepath, const std::string& suffix) {
    if (!stringEndsWith(filepath, suffix)) {
        filepath.append(suffix);
    }

    return filepath;
}

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

std::filesystem::directory_iterator File::getDirContents(std::string path) {
    return std::filesystem::directory_iterator("./" + ASSET_DIR + "/" + ENTITY_DIR + "/" + current_namespace_ + "/" + path);
}

std::optional<nlohmann::json> File::loadJson(std::string filepath) {
    filepath = appendSuffix(filepath, ".json");

    try {
        return nlohmann::json::parse(openFileForInput(filepath));
    } catch (nlohmann::json::parse_error& e) {
        LOGE("Unable to load file %s: %s", filepath.c_str(), e.what());
        return {};
    }
}

std::optional<nlohmann::json> File::loadEntityFromFile() {
    return loadJson(ENTITY_DIR + "/" + current_namespace_ + "/" + ENTITY_FILE);
}

std::ifstream File::openSpriteMapFile(std::string file) {
    return openFileForInput(ENTITY_DIR + "/" + current_namespace_ + "/" + TEXTURE_DIR + "/" + file + ".txt");
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

std::optional<nlohmann::json> File::loadRoomTemplate(std::string filepath) {
    return loadRoom(ROOM_TEMPLATE_DIR + "/" + filepath);
}

std::optional<nlohmann::json> File::loadRoom(std::string filepath) {
    return loadJson(ROOM_DIR + "/" + filepath);
}

std::optional<nlohmann::json> File::loadAiBehavior() {
    return loadJson(ENTITY_DIR + "/" + current_namespace_ + "/" + AI_FILE);
}

std::optional<nlohmann::json> File::loadAnimations() {
    auto file = appendSuffix(ANIMATIONS_FILE, ".json");
    return loadJson(ENTITY_DIR + "/" + current_namespace_ + "/" + file);
}

std::optional<sf::Texture> File::loadTexture(std::string file) {
    std::string filepath;
    // If default constructed (i.e. no entity specified) use outer texture dir
    if (default_constructed_) {
        filepath += TEXTURE_DIR + "/";
    } else {
        filepath += ENTITY_DIR + "/" + current_namespace_ + "/" + TEXTURE_DIR + "/";
    }

    filepath += appendSuffix(file, ".png");

    sf::Texture texture;
    if (!texture.loadFromFile(ASSET_DIR + "/" + filepath)) {
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

std::optional<nlohmann::json> File::loadStates() {
    return loadJson(ENTITY_DIR + "/" + current_namespace_ + "/" + STATE_FILE);
}

std::shared_ptr<sf::Shader> File::loadShader(std::string shader_name) {
    if (!sf::Shader::isAvailable()) {
        LOGW("Shaders unavailable, skipping");
        return {};
    }

    std::shared_ptr<sf::Shader> shader = std::make_shared<sf::Shader>();
    if (!shader->loadFromFile(ASSET_DIR + "/" + SHADER_DIR + "/" + shader_name, sf::Shader::Fragment)) {
        LOGE("Failed to load shader %s", shader_name.c_str());
        throw std::runtime_error("Couldn't load shader");
    }

    return shader;
}

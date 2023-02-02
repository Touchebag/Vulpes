#include "file.h"
#include "utils/log.h"

const std::filesystem::path ASSET_DIR = "assets";
const std::filesystem::path ENTITY_DIR = "entities";
const std::filesystem::path ROOM_DIR = "rooms";
const std::filesystem::path ROOM_TEMPLATE_DIR = "_templates";
const std::filesystem::path TEXTURE_DIR = "textures";
const std::filesystem::path FONT_DIR = "fonts";
const std::filesystem::path SHADER_DIR = "shaders";
const std::filesystem::path CUTSCENE_DIR = "cutscenes";

const std::filesystem::path ENTITY_FILE = "entity";
const std::filesystem::path STATE_FILE = "state";
const std::filesystem::path ANIMATIONS_FILE = "animations";
const std::filesystem::path CUTSCENE_FILE = "cutscene";

const std::filesystem::path SAVE_FILE = "data.sav";

std::stack<std::filesystem::path> File::current_directory_;

File::DirectoryScope File::pushDirectory(std::filesystem::path dir) {
    current_directory_.push(getCurrentDirectory() / dir);

    return DirectoryScope{getCurrentDirectory()};
}

void File::popDirectory(std::filesystem::path dir) {
    if (dir == getCurrentDirectory()) {
        current_directory_.pop();
    } else {
        LOGE("Pop directory: invalid path: %s", dir.string().c_str());
        throw std::runtime_error("");
    }
}

std::filesystem::path File::getCurrentDirectory() {
    if (current_directory_.empty()) {
        return "";
    } else {
        return current_directory_.top();
    }
}

std::filesystem::path File::getEntityDir() {
    return ENTITY_DIR;
}

std::filesystem::path File::getCutsceneDir() {
    return CUTSCENE_DIR;
}

std::filesystem::path File::getFullPath(std::filesystem::path path) {
    return ASSET_DIR / getCurrentDirectory() / path;
}

bool File::isInRoot() {
    return current_directory_.empty();
}

std::ifstream File::openFileForInput(std::filesystem::path filepath) {
    filepath = ASSET_DIR / filepath;
    filepath.make_preferred();

    std::ifstream fs(filepath);

    if (!fs) {
        LOGE("Failed to open file %s for input", filepath.string().c_str());
        exit(1);
    }

    return fs;
}

std::ofstream File::openFileForOutput(std::filesystem::path filepath) {
    filepath = ASSET_DIR / filepath;

    std::ofstream fs(filepath);

    if (!fs) {
        LOGE("Failed to open file %s for output", filepath.string().c_str());
        exit(1);
    }

    return fs;
}

std::filesystem::directory_iterator File::getDirContents(std::filesystem::path path) {
    return std::filesystem::directory_iterator(ASSET_DIR / getCurrentDirectory() / path);
}

std::optional<nlohmann::json> File::loadJson(std::filesystem::path filepath) {
    filepath.replace_extension(".json");

    try {
        return {nlohmann::json::parse(openFileForInput(filepath))};
    } catch (nlohmann::json::parse_error& e) {
        LOGE("Unable to load file %s: %s", filepath.string().c_str(), e.what());
        return std::nullopt;
    }
}

std::optional<nlohmann::json> File::loadEntityFromFile() {
    return loadJson(getCurrentDirectory() / ENTITY_FILE);
}

std::ifstream File::openSpriteMapFile(std::filesystem::path file) {
    auto path = getCurrentDirectory() / TEXTURE_DIR / file;
    path.replace_extension(".txt");

    return openFileForInput(path);
}

bool File::writeJsonToFile(std::filesystem::path filepath, nlohmann::json j) {
    filepath.replace_extension(".json");
    try {
        openFileForOutput(ROOM_DIR / filepath) << j.dump(4);
        return true;
    } catch (nlohmann::json::parse_error& e) {
        LOGE("Unable to save file %s: %s", filepath.string().c_str(), e.what());
        return false;
    }
}

std::optional<nlohmann::json> File::loadRoomTemplate(std::filesystem::path filepath) {
    return loadRoom(ROOM_TEMPLATE_DIR / filepath);
}

std::optional<nlohmann::json> File::loadRoom(std::filesystem::path filepath) {
    return loadJson(ROOM_DIR / filepath);
}

std::optional<nlohmann::json> File::loadAnimations() {
    auto file = ANIMATIONS_FILE;
    file.replace_extension(".json");

    return loadJson(getCurrentDirectory() / file);
}

std::optional<sf::Texture> File::loadTexture(std::filesystem::path file) {
    std::filesystem::path filepath;
    // If default constructed (i.e. no entity specified) use outer texture dir
    if (current_directory_.empty()) {
        filepath = TEXTURE_DIR;
    } else {
        filepath = getCurrentDirectory() / TEXTURE_DIR;
    }

    filepath = filepath / file.replace_extension(".png");

    sf::Texture texture;
    if (!texture.loadFromFile((ASSET_DIR / filepath).string())) {
        return std::nullopt;
    } else {
        return {texture};
    }
}

std::optional<sf::Font> File::loadFont(std::filesystem::path filepath) {
    sf::Font font;
    if (!font.loadFromFile((ASSET_DIR / FONT_DIR / filepath).string())) {
        return std::nullopt;
    } else {
        return {font};
    }
}

std::optional<nlohmann::json> File::loadCutscene() {
    return loadJson(getCurrentDirectory() / CUTSCENE_FILE);
}

std::optional<nlohmann::json> File::loadStates() {
    return loadJson(getCurrentDirectory() / STATE_FILE);
}

std::shared_ptr<sf::Shader> File::loadShader(std::filesystem::path shader_name) {
    if (!sf::Shader::isAvailable()) {
        LOGW("Shaders unavailable, skipping");
        return {};
    }

    std::shared_ptr<sf::Shader> shader = std::make_shared<sf::Shader>();
    if (!shader->loadFromFile((ASSET_DIR / SHADER_DIR / shader_name).string(), sf::Shader::Fragment)) {
        LOGE("Failed to load shader %s", shader_name.string().c_str());
        throw std::runtime_error("Couldn't load shader");
    }

    return shader;
}

nlohmann::json File::loadSaveFile() {
    nlohmann::json save;

    std::ifstream fs(SAVE_FILE);

    if (fs) {
        save = nlohmann::json::parse(fs);
    } else {
        LOGD("No save file found");
    }

    return save;
}

void File::writeSaveFile(nlohmann::json j) {
    std::ofstream fs(SAVE_FILE);

    if (fs) {
        fs << j.dump();
    } else {
        LOGD("Couldn't write to save file");
    }
}

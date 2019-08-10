#include "file.h"
#include "log.h"

#include <fstream>

std::optional<nlohmann::json> file::loadJson(std::string filepath) {
    // TODO Move "assets" prefix in here
    try {
        std::ifstream ifs(filepath);
        return nlohmann::json::parse(ifs);
    } catch (nlohmann::json::parse_error& e) {
        LOGE("Unable to load file %s: %s", filepath.c_str(), e.what());
        return {};
    }
}

bool file::storeJson(std::string filepath, nlohmann::json j) {
    try {
        std::ofstream ofs(filepath);
        ofs << j.dump(4);
        return true;
    } catch (nlohmann::json::parse_error& e) {
        LOGE("Unable to save file %s: %s", filepath.c_str(), e.what());
        return false;
    }
}

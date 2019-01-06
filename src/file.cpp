#include "file.h"
#include "log.h"

#include <fstream>

std::optional<nlohmann::json> file::loadJson(std::string filepath) {
    try {
        std::ifstream ifs(filepath);
        return nlohmann::json::parse(ifs);
    } catch (nlohmann::json::parse_error e) {
        LOGE("Unable to load file %s: %s", filepath.c_str(), e.what());
        return {};
    }
}

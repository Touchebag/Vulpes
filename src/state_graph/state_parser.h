#include "json.hpp"

class StateParser {
  public:
    static std::string parseState(std::string key, nlohmann::json json);
};

#include "json.hpp"

class StateParser {
  public:
    static std::string parseState(nlohmann::json json);
};

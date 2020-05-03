#pragma once

#include <json.hpp>

class Component {
  public:
    virtual void update() = 0;

    virtual void reloadFromJson(nlohmann::json) = 0;
    virtual std::optional<nlohmann::json> outputToJson() = 0;
};

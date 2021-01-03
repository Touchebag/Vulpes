#pragma once

#include <json.hpp>

#include "utils/file.h"

class Component {
  public:
    virtual void update() = 0;

    virtual void reloadFromJson(nlohmann::json) = 0;
    virtual std::optional<nlohmann::json> outputToJson() = 0;
};

class ComponentWithFile : public Component {
  public:
    virtual void reloadFromJson(nlohmann::json, File) = 0;
};

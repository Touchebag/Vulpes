#pragma once

#include <json.hpp>

#include "utils/file.h"

class ComponentStore;

class Component {
  public:
    virtual void update() = 0;

    virtual void reloadFromJson(nlohmann::json, File file_instance = File()) = 0;
    virtual std::optional<nlohmann::json> outputToJson() = 0;

  protected:
    Component(std::weak_ptr<ComponentStore>);

    std::weak_ptr<ComponentStore> component_store_;
};

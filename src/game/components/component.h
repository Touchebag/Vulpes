#pragma once

#include "nlohmann/json.hpp"

#include "utils/file.h"

class ComponentStore;

class Component {
  public:
    virtual ~Component() = default;

    virtual void update() = 0;

    virtual void reloadFromJson(nlohmann::json, File file_instance = File()) = 0;
    virtual std::optional<nlohmann::json> outputToJson() = 0;

    template <typename T>
    std::shared_ptr<T> getComponent() const;

  protected:
    Component(std::weak_ptr<ComponentStore>);

    std::weak_ptr<ComponentStore> component_store_;
};

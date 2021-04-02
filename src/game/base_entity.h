#pragma once

#include <json.hpp>

#include <unordered_set>

#include "utils/common.h"
#include "components/component_store.h"

class BaseEntity {
  public:
    virtual void update();

    void setHitbox(int width, int height);

    static std::shared_ptr<BaseEntity> createFromJson(const nlohmann::json& j);

    virtual void reloadFromJson(const nlohmann::json& j);
    virtual std::optional<nlohmann::json> outputToJson();

    virtual ~BaseEntity() {};

    std::shared_ptr<ComponentStore> components_ = std::make_shared<ComponentStore>();

  private:
    // These are to be able to differentiate between overloaded components
    // and components loaded via entity file when saving
    std::string entity_file_name_;
    nlohmann::json entity_file_exclusives_;
};

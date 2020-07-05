#pragma once

#include "component.h"
#include "subentity.h"

class Death : public Component {
  public:
    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    std::optional<nlohmann::json> getDeathEntityJson();

    void setDead();
    bool isDead();

  private:
    bool is_dead_ = false;

    std::optional<nlohmann::json> death_entity_json_ = std::nullopt;

    std::weak_ptr<Subentity> subentity_;
};

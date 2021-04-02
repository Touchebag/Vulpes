#pragma once

#include "component.h"

class Death : public Component {
  public:
    Death(std::weak_ptr<ComponentStore> components);

    void update() override;

    static std::shared_ptr<Death> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    std::optional<nlohmann::json> getDeathEntityJson();

    void setDead();
    bool isDead();

  private:
    bool is_dead_ = false;

    std::optional<nlohmann::json> death_entity_json_ = std::nullopt;
};

#pragma once

#include "components/component.h"
#include "components/transform.h"
#include "components/movement.h"

class BaseEntity;

class Subentity : public Component {
  public:
    Subentity(std::weak_ptr<ComponentStore> components);

    void update() override;

    static std::shared_ptr<Subentity> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    void addEntity(std::shared_ptr<BaseEntity> entity);

  private:
    void set_position();

    std::weak_ptr<BaseEntity> entity_;
};

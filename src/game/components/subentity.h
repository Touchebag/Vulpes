#pragma once

#include "components/component.h"
#include "components/transform.h"

class BaseEntity;

class Subentity : public Component {
  public:
    Subentity(std::weak_ptr<Transform> trans);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void addEntity(std::shared_ptr<BaseEntity> entity);

  private:
    std::weak_ptr<Transform> trans_;

    std::weak_ptr<BaseEntity> entity_;
};

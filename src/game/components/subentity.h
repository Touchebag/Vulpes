#pragma once

#include "components/component.h"
#include "components/transform.h"
#include "components/rendering/rendering.h"

class BaseEntity;

class Subentity : public Component {
  public:
    Subentity(std::weak_ptr<Transform> trans, std::weak_ptr<RenderableEntity> render);

    void update() override;

    void reloadFromJson(nlohmann::json j) override;
    std::optional<nlohmann::json> outputToJson() override;

    void addEntity(std::shared_ptr<BaseEntity> entity);

  private:
    void set_position();

    std::weak_ptr<Transform> trans_;
    std::weak_ptr<RenderableEntity> render_;

    std::weak_ptr<BaseEntity> entity_;
};

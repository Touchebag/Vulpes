#pragma once

#include <memory>

#include "components/rendering/rendering.h"
#include "components/actions/actions.h"
#include "components/animation.h"
#include "components/movement.h"
#include "components/collision/collision.h"

#include "components/physics/constants.h"

class Physics : public Component {
  public:
    explicit Physics(std::weak_ptr<ComponentStore> components);

    void update() override;

    void setPhysicsConstants(PhysicsConstants constants);
    void setPhysicsConstants();
    PhysicsConstants getPhysicsConstants();

    void resetJumps(int max_jumps);
    void resetDashes(int max_dashes);

    static std::shared_ptr<Physics> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());

    static PhysicsConstants loadConstantsFromJson(nlohmann::json j, PhysicsConstants default_constants);
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

  private:
    PhysicsConstants constants_;
    PhysicsConstants original_constants_;

    int jumps_left_ = 0;
    int dashes_left_ = 0;
};

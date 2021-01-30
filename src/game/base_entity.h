#pragma once

#include <json.hpp>

#include <unordered_set>

#include "utils/common.h"

#include "components/transform.h"
#include "components/movement.h"
#include "components/rendering/rendering.h"
#include "components/animation.h"
#include "components/stateful.h"
#include "components/actions/actions.h"
#include "components/collision/collision.h"
#include "components/physics/physics.h"
#include "components/ai.h"
#include "components/damageable/damageable.h"
#include "components/subentity.h"
#include "components/death.h"

class BaseEntity {
  public:
    virtual void update();

    void setHitbox(int width, int height);

    static std::shared_ptr<BaseEntity> createFromJson(nlohmann::json j);

    virtual void reloadFromJson(nlohmann::json j);
    virtual std::optional<nlohmann::json> outputToJson();

    virtual ~BaseEntity() {};

    // Components
    std::shared_ptr<Transform> trans_ = nullptr;
    std::shared_ptr<Collision> collision_ = nullptr;
    std::shared_ptr<MovableEntity> movableEntity_ = nullptr;
    std::shared_ptr<RenderableEntity> renderableEntity_ = nullptr;
    std::shared_ptr<AnimatedEntity> animatedEntity_ = nullptr;
    std::shared_ptr<StatefulEntity> statefulEntity_ = nullptr;
    std::shared_ptr<Actions> actions_ = nullptr;
    std::shared_ptr<Physics> physics_ = nullptr;
    std::shared_ptr<AI> ai_ = nullptr;
    std::shared_ptr<Damageable> damageable_ = nullptr;
    std::shared_ptr<Subentity> subentity_ = nullptr;
    std::shared_ptr<Death> death_ = nullptr;

  private:
    std::string entity_file_;

    // Stores which components are loaded from entity file
    // This will allow overloading certain components locally without affecting
    // the main file
    std::unordered_set<std::string> file_loaded_components_;
};

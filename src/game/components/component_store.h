#pragma once

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

class ComponentStore {
  public:
    template <typename T>
    std::shared_ptr<T> getComponent() const;
    template <typename T>
    void setComponent(std::shared_ptr<T> component);

  private:
    // Components
    std::shared_ptr<Transform> transform = nullptr;
    std::shared_ptr<Collision> collision = nullptr;
    std::shared_ptr<Movement> movement = nullptr;
    std::shared_ptr<Rendering> rendering = nullptr;
    std::shared_ptr<Animation> animation = nullptr;
    std::shared_ptr<Stateful> stateful = nullptr;
    std::shared_ptr<Actions> actions = nullptr;
    std::shared_ptr<Physics> physics = nullptr;
    std::shared_ptr<AI> ai = nullptr;
    std::shared_ptr<Damageable> damageable = nullptr;
    std::shared_ptr<Subentity> subentity = nullptr;
    std::shared_ptr<Death> death = nullptr;
};

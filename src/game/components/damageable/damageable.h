#pragma once

#include <memory>

#include "json.hpp"

#include "components/collision/collision.h"
#include "components/death.h"
#include "components/stateful.h"
#include "components/movement.h"

class Damageable : public Component {
  public:
    Damageable(std::weak_ptr<ComponentStore> components);

    // Component interface
    void reloadFromJson(nlohmann::json j, File file_instance = File()) override;
    virtual std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    // Load derived classes from json
    static std::shared_ptr<Damageable> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());

    int getHealth();
    bool isDead();

  protected:
    int health_ = 0;

  private:
    int invincibility_frame_counter_ = 0;

    // For output to json
    int initial_health_;
};

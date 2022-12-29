#pragma once

#include "components/actions/actions.h"
#include "components/collision/collision.h"
#include "components/transform.h"

#include "program.h"

class AI : public Component {
  public:
    explicit AI(std::weak_ptr<ComponentStore> components);

    void update() override;

    static std::shared_ptr<AI> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file_instance = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    void executeProgram(const Program& program);

  private:
    unsigned int frame_timer_ = 0;
};
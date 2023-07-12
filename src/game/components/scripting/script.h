#pragma once

#include "components/actions/actions.h"
#include "components/collision/collision.h"
#include "components/transform.h"

#include "program.h"

class Scripting : public Component {
  public:
    explicit Scripting(std::weak_ptr<ComponentStore> components);

    void update() override;

    static std::shared_ptr<Scripting> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file_instance = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    void executeProgram(const Program& program);

  private:
    unsigned int frame_timer_ = 0;

    std::shared_ptr<scripting::VariableMap> variables_ = std::make_shared<scripting::VariableMap>();
};

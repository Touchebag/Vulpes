#pragma once

#include "actions.h"

class ActionsPlayer : public Actions {
  public:
    ActionsPlayer(std::weak_ptr<ComponentStore> components);

    static std::shared_ptr<ActionsPlayer> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file_instance = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    void update() override;

    void addAction(Action action) override;
};

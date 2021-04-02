#pragma once

#include <json.hpp>

#include "component.h"

#include "utils/common.h"

class Transform : public Component {
public:
    Transform(std::weak_ptr<ComponentStore> components);

    void setPosition(const int x, const int y);
    void setPosition(util::Point point);
    util::Point getPosition();

    void update() override;

    static std::shared_ptr<Transform> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    int getX() const;
    int getY() const;

private:
    int pos_x_ = 0;
    int pos_y_ = 0;
};

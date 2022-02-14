#pragma once

#include "component.h"

#include "utils/common.h"

class Transform : public Component {
public:
    explicit Transform(std::weak_ptr<ComponentStore> components);

    void setPosition(const int x, const int y);
    void setPosition(util::Vec2i point);
    util::Vec2i getPosition() const;

    void setRawPosition(util::Vec2d point);
    util::Vec2d getRawPosition() const;

    void update() override;

    static std::shared_ptr<Transform> createFromJson(nlohmann::json, std::weak_ptr<ComponentStore>, File file_instance = File());
    void reloadFromJson(nlohmann::json j, File file = File()) override;
    std::optional<nlohmann::json> outputToJson() override;

    int getX() const;
    int getY() const;

private:
    double pos_x_ = 0.0;
    double pos_y_ = 0.0;
};

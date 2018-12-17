#pragma once

#include "base_entity.h"

class Player : public BaseEntity {
    public:
        void update() override;

    private:
        void moveAndCheckCollision(float x, float y);
};

#pragma once

#include "base_entity.h"
#include "movable_entity.h"

class Player : public BaseEntity,
               public MovableEntity {
    public:
        void update() override;

        void move(vec2d velocity) override;

    private:
        void moveAndCheckCollision();
};

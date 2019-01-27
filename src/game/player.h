#pragma once

#include "base_entity.h"
#include "movable_entity.h"
#include "stateful_entity.h"
#include "animated_entity.h"

class Player : public BaseEntity,
               public MovableEntity,
               public StatefulEntity,
               public AnimatedEntity {
    public:
        void update() override;

        void move(util::X velX, util::Y velY) override;

    private:
        void moveAndCheckCollision();
};

#include "operation.h"

#include "render.h"

void Operation::undo() {
    if (entity_) {
        if (!before_) {
            // If entity did not exist before, delete it
            World::getInstance().removeEntity(entity_, layer_);
        } else if (!after_) {
            // If entity does not exist after, add it
            World::getInstance().addEntity(entity_, layer_);
        } else {
            entity_->loadFromJson(before_.value());
            Render::getInstance().addEntity(entity_->renderableEntity_, layer_);
        }
    }
}

void Operation::redo() {
    if (entity_) {
        if (!before_) {
            // If entity did not exist before it was added
            World::getInstance().addEntity(entity_, layer_);
        } else if (!after_) {
            // If entity does not exist after it was removed
            World::getInstance().removeEntity(entity_, layer_);
        } else {
            entity_->loadFromJson(after_.value());
            Render::getInstance().addEntity(entity_->renderableEntity_, layer_);
        }
    }
}

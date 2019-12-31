#include "operation.h"

#include "system/render.h"

void Operation::undo() {
    if (entity_) {
        if (!before_) {
            // If entity did not exist before, delete it
            entity_->renderableEntity_.reset();
            World::getInstance<World::IWorldModify>().removeEntity(entity_, layer_);
        } else if (!after_) {
            // If entity does not exist after, add it
            entity_->loadFromJson(before_.value());
            World::getInstance<World::IWorldModify>().addEntity(entity_, layer_);
        } else {
            entity_->loadFromJson(before_.value());
            Render::getInstance().addEntity(entity_->renderableEntity_, layer_);
        }
    }
}

void Operation::redo() {
    if (entity_) {
        if (!after_) {
            // If entity does not exist after it was removed
            entity_->renderableEntity_.reset();
            World::getInstance<World::IWorldModify>().removeEntity(entity_, layer_);
        } else if (!before_) {
            // If entity did not exist before it was added
            entity_->loadFromJson(after_.value());
            World::getInstance<World::IWorldModify>().addEntity(entity_, layer_);
        } else {
            entity_->loadFromJson(after_.value());
            Render::getInstance().addEntity(entity_->renderableEntity_, layer_);
        }
    }
}

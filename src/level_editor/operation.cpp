#include "operation.h"

#include "system/system.h"

void Operation::undo() {
    if (entity_) {
        if (!before_) {
            // If entity did not exist before, delete it
            entity_->setComponent<RenderableEntity>({});
            World::getInstance<World::IWorldModify>().removeEntity(entity_);
        } else if (!after_) {
            // If entity does not exist after, add it
            entity_->reloadFromJson(before_.value());
            World::getInstance<World::IWorldModify>().addEntity(entity_);
        } else {
            entity_->reloadFromJson(before_.value());
            System::getRender()->addEntity(entity_->getComponent<RenderableEntity>());
        }
    }
}

void Operation::redo() {
    if (entity_) {
        if (!after_) {
            // If entity does not exist after it was removed
            entity_->setComponent<RenderableEntity>({});
            World::getInstance<World::IWorldModify>().removeEntity(entity_);
        } else if (!before_) {
            // If entity did not exist before it was added
            entity_->reloadFromJson(after_.value());
            World::getInstance<World::IWorldModify>().addEntity(entity_);
        } else {
            entity_->reloadFromJson(after_.value());
            System::getRender()->addEntity(entity_->getComponent<RenderableEntity>());
        }
    }
}

#include "add_entity.h"

#include "system/system.h"

namespace event_triggers {

AddEntity::AddEntity(std::shared_ptr<BaseEntity> entity) :
    entity_(entity) {
}

void AddEntity::onEvent() {
    System::IWorldModify::addEntity(entity_);
}

} // event_triggers

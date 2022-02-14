#pragma once

#include "event_trigger.h"

#include "base_entity.h"

namespace event_triggers {

class AddEntity : public IEventTrigger {
  public:
    explicit AddEntity(std::shared_ptr<BaseEntity> entity);

    void onEvent() override;

  private:
    std::shared_ptr<BaseEntity> entity_;
};

} // event_triggers

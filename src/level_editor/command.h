#pragma once

#include <memory>

#include "history.h"
#include "mouse.h"

#include "operations/operation.h"

class Command {
  public:
    Command(std::shared_ptr<History> history,
            std::shared_ptr<BaseEntity> current_entity,
            std::shared_ptr<Operation> current_operation,
            std::shared_ptr<Mouse> mouse);

    void setLayer(World::Layer layer);

    void add();

    World::Layer current_layer_ = World::Layer::MAIN;

  private:
    std::shared_ptr<History> history_;
    std::shared_ptr<BaseEntity> current_entity_;
    std::shared_ptr<Operation> current_operation_;
    std::shared_ptr<Mouse> mouse_;
};

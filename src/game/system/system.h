#pragma once

#include <memory>

#include "render.h"

class System {
  public:
    static void setRender(std::shared_ptr<IRender> render);
    static std::shared_ptr<IRender> getRender();

  private:
    static System& getInstance();

    std::shared_ptr<IRender> render_instance_ = std::make_shared<Render>();
};

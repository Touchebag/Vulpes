#pragma once

#include <memory>

#include "render.h"
#include "cutscene_handler.h"

class System {
  public:
    static void setRender(std::shared_ptr<IRender> render);
    static std::shared_ptr<IRender> getRender();

    static std::shared_ptr<CutsceneHandler> getCutscene();

  private:
    static System& getInstance();

    std::shared_ptr<IRender> render_instance_ = std::make_shared<Render>();
    std::shared_ptr<CutsceneHandler> cutscene_handler_ = std::make_shared<CutsceneHandler>();
};

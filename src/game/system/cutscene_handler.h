#pragma once

#include "nlohmann/json.hpp"

#include "cutscene.h"

class CutsceneHandler {
  public:
    void startCutscene(std::string name);

    bool isCutscenePlaying();

    void update();

  private:
    void execute_event(const Cutscene::CutsceneEvent& event);

    std::shared_ptr<Cutscene> current_cutscene_;

    unsigned int frame_counter_;
    unsigned int next_event_frame;
};

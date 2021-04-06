#include "system.h"

void System::setRender(std::shared_ptr<IRender> render) {
    System::getInstance().render_instance_ = render;
}

std::shared_ptr<IRender> System::getRender() {
    auto render = System::getInstance().render_instance_;
    if (!render) {
        throw std::runtime_error("System: render instance empty");
    }

    return render;
}

std::shared_ptr<Input> System::getInput() {
    return System::getInstance().input_;
}

std::shared_ptr<Camera> System::getCamera() {
    return System::getInstance().camera_instance_;
}

std::shared_ptr<CutsceneHandler> System::getCutscene() {
    return System::getInstance().cutscene_handler_;
}

System& System::getInstance() {
    static System system;

    return system;
}

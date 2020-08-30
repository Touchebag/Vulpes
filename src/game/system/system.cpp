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

System& System::getInstance() {
    static System system;

    return system;
}

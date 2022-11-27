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

void System::setCamera(std::shared_ptr<Camera> camera) {
    System::getInstance().camera_instance_ = camera;
}

std::shared_ptr<Camera> System::getCamera() {
    return System::getInstance().camera_instance_;
}

void System::setEnvironment(std::shared_ptr<Environment> environment) {
    System::getInstance().environment_ = environment;
}

std::shared_ptr<Environment> System::getEnvironment() {
    return System::getInstance().environment_;
}

System& System::getInstance() {
    static System system;

    return system;
}

void System::setWorld(std::shared_ptr<World> world) {
    System::getInstance().world_ = world;
}

std::shared_ptr<World> System::getWorld() {
    return System::getInstance().world_;
}

void System::setCutscene(std::shared_ptr<Cutscene> cutscene) {
    System::getInstance().cutscene_ = cutscene;
    cutscene->start();
}

std::shared_ptr<Cutscene> System::getCutscene() {
    return System::getInstance().cutscene_;
}
